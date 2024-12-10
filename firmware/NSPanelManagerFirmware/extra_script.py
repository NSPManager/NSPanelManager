import os
import shutil
Import("env")

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
env.Replace(COMPILATIONDB_PATH=os.path.join(
    "$BUILD_DIR", "compile_commands.json"))

# Copy all EmbeddedProto src files to path included in PlatformIO
for filename in os.listdir("../../docker/protobuf/EmbeddedProto/src/"):
    src_file = os.path.join(
        "../../docker/protobuf/EmbeddedProto/src/", filename)
    dest_file = os.path.join("lib/EmbeddedProto", filename)

    if (os.path.isfile(src_file)):
        shutil.copy(src_file, dest_file)  # Copy file, overwrite if existing
