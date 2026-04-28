FLAGS = -D _DEBUG -D _MY_STRCMP -march=native -g -O3 -ggdb3 -std=c++17 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -Werror=vla 

CXX = g++

SOURCES =  main_argc.cpp hash_functions.cpp hash_table.cpp file_functions.cpp management_functions.cpp
OBJECTS = $(patsubst %.cpp, %.o, $(SOURCES)) my_strcmp.o

all: hash_table_my_strcmp_asm_inline.exe

%.o: %.cpp
	@$(CXX) -g -c $^ -o $@ $(FLAGS)

my_strcmp.o: my_strcmp.asm
	nasm -f elf64 my_strcmp.asm -o my_strcmp.o

hash_table_my_strcmp_asm_inline.exe: $(OBJECTS)
	@$(CXX) -g $^ -o $@ $(FLAGS)

.PHONY: clean

clean:
	rm *.o
	rm hash_table_my_strcmp_asm_inline.exe