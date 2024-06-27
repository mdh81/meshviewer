import glob
import os
import shutil
import git
import sys
import traceback
from enum import Enum


ARTIFACT_DIR = 'artifacts'
BUILD_DIR = 'build_web'
DEPLOYMENT_DIR = 'deployment'
DEPLOY_URL = 'https://github.com/mdh81/3dviewer.git'
SOURCE_URL = 'https://github.com/mdh81/meshviewer/commit/'
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
# NOTE: Change this if the corresponding artifact in WEB_VIEWER_ARTIFACTS changes
MAIN_HTML = 'meshViewer.html'
SHORT_SHA_LENGTH = 7


class Mode(Enum):
    Assemble = 'assembly'
    Deploy = 'deploy'


def print_usage_and_exit() -> None:
    print(f'Usage: {sys.argv[0]} <mode: assemble|deploy> <version id (only for the deploy mode)>')
    sys.exit(-1)


def check_arguments() -> Mode:
    if len(sys.argv) < 2 or sys.argv[1] not in ('assemble', 'deploy'):
        print_usage_and_exit()
    execution_mode: Mode = Mode.Assemble if sys.argv[1] == 'assemble' else Mode.Deploy
    if execution_mode is Mode.Deploy and len(sys.argv) != 3:
        print_usage_and_exit()
    return execution_mode


def check_artifacts() -> None:
    build_dir = os.path.join(os.path.curdir, BUILD_DIR)
    if not os.path.exists(build_dir):
        raise RuntimeError(f'{build_dir} does not exist')

    for artifact in WEB_VIEWER_ARTIFACTS:
        if not os.path.exists(os.path.join(build_dir, artifact)):
            raise RuntimeError(f'{artifact} not found in {build_dir}')


def assemble():
    os.makedirs(ARTIFACT_DIR, exist_ok=True)

    build_dir = os.path.join(os.path.curdir, BUILD_DIR)
    for item in WEB_VIEWER_ARTIFACTS:
        artifact = os.path.join(build_dir, item)
        print(f'Copying {artifact} to {ARTIFACT_DIR}')
        if os.path.isdir(artifact):
            shutil.copytree(artifact, os.path.join(ARTIFACT_DIR, item), dirs_exist_ok=True)
        else:
            artifact_dir_name = os.path.dirname(os.path.normpath(artifact))
            if artifact_dir_name == BUILD_DIR:
                # artifact exists in root of the build directory
                shutil.copy2(artifact, ARTIFACT_DIR)
            else:
                # artifact exists in a subdirectory of the build directory
                # get the subdirectory path relative to the build directory and create all directories in the path
                # hierarchy before copying the file into the newly created directory structure
                rel_path = os.path.relpath(artifact, start=os.path.commonpath([artifact, BUILD_DIR]))
                sub_dir = os.path.dirname(os.path.join(ARTIFACT_DIR, rel_path))
                os.makedirs(sub_dir, exist_ok=True)
                shutil.copy2(artifact, sub_dir)

    print(f'All web viewer artifacts copied to {ARTIFACT_DIR}')


def deploy():
    # Create deployment directory
    if os.path.exists(DEPLOYMENT_DIR):
        shutil.rmtree(DEPLOYMENT_DIR)
    os.makedirs(DEPLOYMENT_DIR)

    # Create local git repository whose origin is the 3dviewer
    gh_token = os.getenv('GITHUB_TOKEN')
    if not gh_token:
        raise RuntimeError('Missing environment variable GITHUB_TOKEN. Need a token to authenticate with the '
                           'deployment repository')
    repo = git.Repo.clone_from(DEPLOY_URL.replace('https://', f'https://{gh_token}@'), DEPLOYMENT_DIR, branch='main')
    origin = repo.remote(name='origin')

    # Copy artifacts to deployment directory
    artifacts = os.listdir(ARTIFACT_DIR)
    for artifact in artifacts:
        artifact_path = os.path.join(ARTIFACT_DIR, artifact)
        if os.path.isdir(artifact_path):
            shutil.copytree(artifact_path, os.path.join(DEPLOYMENT_DIR, artifact), dirs_exist_ok=True)
        else:
            shutil.copy2(artifact_path, DEPLOYMENT_DIR)

    # Create index.html
    if os.path.exists(os.path.join(DEPLOYMENT_DIR, MAIN_HTML)):
        os.rename(os.path.join(DEPLOYMENT_DIR, MAIN_HTML), os.path.join(DEPLOYMENT_DIR, 'index.html'))
    else:
        raise RuntimeError(f'Expected artifact {MAIN_HTML} was not found in deployment directory')

    # Push the artifacts to remote
    repo.git.add(all=True)
    repo.index.commit(f'Updating web viewer to version {sys.argv[3][:SHORT_SHA_LENGTH]} ({SOURCE_URL}/{sys.argv[3]})')
    origin.push()


if __name__ == '__main__':
    mode: Mode = check_arguments()
    try:
        if mode == Mode.Assemble:
            check_artifacts()
            assemble()
        else:
            deploy()
    except Exception as error:
        print(f'\nError during artifacts {mode.value} phase: {error}\n\n{traceback.format_exc()}')
        sys.exit(-1)
