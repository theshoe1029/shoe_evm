keccak:
	gcc keccak256.c keccak256_runner.c -o keccak256

rlp:
	gcc rlp.c rlp_runner.c -o rlp
hp:
	gcc hex_prefix.c hex_prefix_runner.c -o hp
trie:
	gcc -I ./include hex_prefix.c rlp.c keccak256.c trie.c trie_runner.c -o trie
leveldb:
	git clone --recurse-submodules https://github.com/google/leveldb.git && cd leveldb && mkdir -p build && cd build
	cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
clean:
	rm -rf keccak256
	rm -rf rlp
	rm -rf hp
	rm -rf trie