from os import system,cpu_count,path
PROJECT_NAME:str="sdl"
BUILD_DATE_TIME:str="Thu Feb 06 03:31:36 2025"
#Warning : DO NOT REMOVE THIS LINE OR ADD ANYTHING ABOVE EVEN IMPORT SHOULD GO BELOW THIS LINE
GENERATOR:str="\"Ninja\""
conan_file_exist:bool=path.exists("conanfile.txt")
if __name__=="__main__":
    if not conan_file_exist:
        print('Compilling SDL...')
        if not system(f"cd SDL && ninja -C build -j{cpu_count()}"):
            exit(0)
        if not system(f"cd SDL && cmake -S . -B build  -G {GENERATOR} -DBUILD_SHARED_LIBS=OFF"):
            system(f"cmake --install external/SDL/build --prefix external/SDL/install")

    else:
        print("failed!")
