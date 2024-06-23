#
# c1 2024
#

# defines
TARGET    := ncache
VERSION   := 1.0 ($(shell date +%Y-%m-%d))
LINKFLAGS := -s -w -X 'main.version=$(VERSION)'
BINNAME   := bin/$(TARGET)

# rename for release (cross)builds
ifeq ($(CROSSBUILD),1)
	ifeq (${GOOS}, windows)
		BINNAME := $(BINNAME)-${GOARCH}.exe
	else
		ifeq (${GOOS}, darwin)
			BINNAME := $(BINNAME)-mac-${GOARCH}
		else
			BINNAME := $(BINNAME)-${GOOS}-${GOARCH}
		endif
	endif
endif

# verbosity
Q = @
V = @echo
ifeq ($(VERBOSE),1)
	Q =
	V = @:
endif
invoke-build := $(MAKE) --no-print-directory build CROSSBUILD=1

.PHONY: build run install release all clean deps tidy

build: main.go
	${V} "Building $(BINNAME)..."
	${Q}go build -v -ldflags="$(LINKFLAGS)" -o $(BINNAME) $^

run: main.go
	${Q}go run $^ --pretty -o ""

install: main.go
	${V} "Installing..."
	${Q}go install -ldflags="$(LINKFLAGS)" ./...

release: main.go
	@echo "Compiling version $(VERSION) for Release:"
	@echo "Linux:"
	@CGO_ENABLED=0 GOOS=linux GOARCH=amd64 GOAMD64=v2 $(invoke-build)
	@CGO_ENABLED=0 GOOS=linux GOARCH=arm $(invoke-build)
	@CGO_ENABLED=0 GOOS=linux GOARCH=arm64 $(invoke-build)
	@echo "Windows:"
	@CGO_ENABLED=0 GOOS=windows GOARCH=amd64 $(invoke-build)
	@echo "macOS:"
	@CGO_ENABLED=0 GOOS=darwin GOARCH=amd64 $(invoke-build)
	@CGO_ENABLED=0 GOOS=darwin GOARCH=arm64 $(invoke-build)

all: build

clean:
	${V} "Cleaning..."
	${Q}rm -rf bin
	${Q}rm -f index.json
	${Q}go clean

deps:
	${V} "Downloading..."
	${Q}go mod download

tidy:
	${V} "Tidying..."
	${Q}go fmt ./...
	${Q}go mod tidy -v
