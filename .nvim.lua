local clangd_profiles = {
  native = "cmake-build-debug",
  release = "cmake-build-release",
  wasm = "build_web_Release",
}

-- Resolve build directories relative to this project, regardless of where
-- Neovim was launched.
local project_root = vim.fn.fnamemodify(
  debug.getinfo(1, "S").source:sub(2),
  ":p:h"
)

local function clangd_cmd(profile)
  local relative_build_dir = clangd_profiles[profile]
  assert(relative_build_dir, "Unknown clangd profile: " .. profile)

  local build_dir = vim.fs.joinpath(project_root, relative_build_dir)

  return {
    "clangd",
    "--background-index",
    "--clang-tidy",
    "--compile-commands-dir=" .. build_dir,
  }, build_dir
end

local active_profile = vim.env.CLANGD_PROFILE or "native"

vim.lsp.config("clangd", {
  cmd = clangd_cmd(active_profile),
  root_dir = project_root,
  filetypes = { "c", "cpp", "objc", "objcpp", "cuda" },
})

vim.lsp.enable("clangd")

vim.api.nvim_create_user_command("ClangdProfile", function(opts)
  local profile = opts.args
  local cmd, build_dir = clangd_cmd(profile)
  local database = vim.fs.joinpath(build_dir, "compile_commands.json")

  if vim.fn.filereadable(database) == 0 then
    vim.notify(
      "Missing compilation database: " .. database,
      vim.log.levels.ERROR
    )
    return
  end

  active_profile = profile

  vim.lsp.enable("clangd", false)
  vim.lsp.config("clangd", { cmd = cmd })
  vim.lsp.enable("clangd")

  vim.notify("clangd profile: " .. active_profile)
end, {
  nargs = 1,
  complete = function()
    return vim.tbl_keys(clangd_profiles)
  end,
})
