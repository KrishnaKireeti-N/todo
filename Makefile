src := $(wildcard src/*.c)
output := bin/todo

all: bin debug clean run

# To make todo 'commandable' follow these commands (make sure pwd is the repo's dir)
# make release
# ln -s $(pwd)/bin/todo ~/.local/bin/todo
bin:
	@mkdir -p bin

# ------------------------------------------------------------------
compileflags := -g -std=c2x -Iinclude ${src} -o ${output}

debugflags := -O0 -Wall -Wshadow -Werror
debug:
	@$(CC) ${compileflags} ${debugflags}

releaseflags = -O2
release:
	@$(CC) ${compileflags} ${releaseflags}
#-------------------------------------------------------------------

clean:
	@rm -rf bin

run:
	./${output}
