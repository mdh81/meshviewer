mkdir -p artifacts
cp build_web/meshViewer.* artifacts/
cp -r build_web/js artifacts/
cp -r build_web/shaders artifacts/
mv artifacts/meshViewer.html artifacts/index.html