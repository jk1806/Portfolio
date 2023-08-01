CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lmbedtls -lmbedx509 -lmbedcrypto

SRCDIR = .
BUILDDIR = build

.PHONY: all clean test

all: $(BUILDDIR)/firmware $(BUILDDIR)/ota_client

$(BUILDDIR)/firmware: firmware/src/main.c crypto/src/*.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/ota_client: ota_client/src/*.c crypto/src/*.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(BUILDDIR)

test:
	@echo "Running tests..."
	@cd tests && ./run_tests.sh

