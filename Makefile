keccak:
	gcc keccak256.c keccak256_runner.c -o keccak256

rlp:
	gcc rlp.c rlp_runner.c -o rlp
clean:
	rm -rf keccak256
	rm -rf rlp