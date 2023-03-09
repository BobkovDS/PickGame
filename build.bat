@echo ON

mkdir build
cd build

::Запускаем генерацию проекта
cmake -G "Visual Studio 17 2022" -A x64 ../sources -DCMAKE_BUILD_TYPE=Debug -DAP_EXPORT_TEST_ROWSET_CONSUMER=ON > _cmakeout.log 2>&1
robocopy  ..\..\..\FBXSDK\  ./FBXLoader /e

popd