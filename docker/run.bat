@echo OFF
docker run --rm -it --cpus 4 --cap-add=SYS_PTRACE --security-opt seccomp=unconfined --name="cs2200" -v "%CD%\workspace":/cs2200 -w="/cs2200" cs2200image