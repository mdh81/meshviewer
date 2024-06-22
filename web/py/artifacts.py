import glob
import os
import shutil
import sys
import traceback
from enum import Enum


ARTIFACT_DIR = 'artifacts'
WEB_BUILD_DIR = 'build_web'
BUILD_DIR = 'build'

# NOTE: Paths in this list must be relative to the web viewer's build directory
WEB_VIEWER_ARTIFACTS = [
    'meshViewer.html',
    'meshViewer.data',
    'meshViewer.js',
    'meshViewer.wasm',
    'meshViewer.worker.js',
    'shaders/shaders.html',
    'js/'
]


class Mode(Enum):
    Assemble = 'assembly'
    Deploy = 'deploy'


def print_usage_and_exit() -> None:
    print(f'Usage: {sys.argv[0]} <mode: assemble|deploy>')
    sys.exit(-1)


def check_arguments(args) -> Mode:
    if len(args) != 2:
        print_usage_and_exit()
    if sys.argv[1] not in ('assemble', 'deploy'):
        print_usage_and_exit()
    return Mode.Assemble if sys.argv[1] == 'assemble' else Mode.Deploy


def check_artifacts() -> None:
    build_dir = os.path.join(os.path.curdir, WEB_BUILD_DIR)
    if not os.path.exists(build_dir):
        raise RuntimeError(f'{build_dir} does not exist')

    for artifact in WEB_VIEWER_ARTIFACTS:
        if not os.path.exists(os.path.join(build_dir, artifact)):
            raise RuntimeError(f'{artifact} not found in {build_dir}')


def assemble():
    os.makedirs(ARTIFACT_DIR, exist_ok=True)

    build_dir = os.path.join(os.path.curdir, WEB_BUILD_DIR)
    for item in WEB_VIEWER_ARTIFACTS:
        artifact = os.path.join(build_dir, item)
        print(f'Copying {artifact} to {ARTIFACT_DIR}')
        if os.path.isdir(artifact):
            shutil.copytree(artifact, os.path.join(ARTIFACT_DIR, artifact), dirs_exist_ok=True)
        else:
            artifact_dir_name = os.path.dirname(os.path.normpath(artifact))
            if artifact_dir_name == WEB_BUILD_DIR:
                # artifact exists in root of the build directory
                shutil.copy2(artifact, ARTIFACT_DIR)
            else:
                # artifact exists in a subdirectory of the build directory
                # get the subdirectory path relative to the build directory and create all directories in the path
                # hierarchy before copying the file into the newly created directory structure
                rel_path = os.path.relpath(artifact, start=os.path.commonpath([artifact, WEB_BUILD_DIR]))
                sub_dir = os.path.dirname(os.path.join(ARTIFACT_DIR, rel_path))
                os.makedirs(sub_dir, exist_ok=True)
                shutil.copy2(artifact, sub_dir)

    print(f'All web viewer artifacts copied to {ARTIFACT_DIR}')


def cleanup(cleanup_mode: Mode) -> None:
    if cleanup_mode == Mode.Assemble:
        shutil.rmtree(WEB_BUILD_DIR)
        shutil.rmtree(BUILD_DIR)
    elif cleanup_mode == Mode.Deploy:
        shutil.rmtree(ARTIFACT_DIR)
    else:
        raise ValueError(f'Unknown cleanup mode {cleanup_mode}')


def deploy():
    artifacts = glob.glob(f'{ARTIFACT_DIR}/*')
    for artifact in artifacts:
        shutil.copy2(artifact, '.')
    print(f'Artifacts copied to root directory')


if __name__ == '__main__':
    mode: Mode = check_arguments(sys.argv)
    try:
        if mode == Mode.Assemble:
            check_artifacts()
            assemble()
            cleanup(mode)
        else:
            deploy()
            cleanup(mode)
    except Exception as error:
        print(f'\nError during artifacts {mode.value} phase: {error}\n\n{traceback.format_exc()}')
        sys.exit(-1)
