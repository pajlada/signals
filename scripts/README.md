These scripts are purely for development. They are not necessary for packaging or building.

Run `./scripts/docker-build.sh` to build docker images under `pajlada-signals:` that contain your source dir.

Run them with `./scripts/docker-test.sh` and they will all build to ensure things build & test as expected.

Run them with `./scripts/docker-example.sh` and they will make sure the library can be included using SOURCE_DIR (current source code from your repository), GIT (latest main branch from github), and FIND_PACKAGE (system installed)

There's no error reporting, but `rg -i 'error' logs/*.txt` is good about reporting things.
