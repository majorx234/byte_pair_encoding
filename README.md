# INfo 
- WIP
- implement byte pair encoding

# Build
- `mkdir -p build; cd build`
- `cmake ..`
- `make`

# Usage
- `./byte_pair_encode "The original BPE algorithm operates by iteratively replacing the most common contiguous sequences of characters in a target text with unused 'placeholder' bytes. The iteration ends when no sequences can be found, leaving the target text effectively compressed. Decompression can be performed by reversing this process, querying known placeholder terms against their corresponding denoted sequence, using a lookup table. In the original paper, this lookup table is encoded and stored alongside the compressed text."`

# ToDo:
- CLI
  - save encoding to file (atm hard coded `pairs.dat`
  - optional output to dot-graph (atm hardcoded `pairs.dot`
  - load able endcoding from file (handling via cli argument is needed)

# References
- https://en.wikipedia.org/wiki/Byte_pair_encoding
- Tsodings LLM Tokenizer:
  - https://www.youtube.com/watch?v=6dCqR9p0yWY
- Hashmap Implementation by nothings:
  - https://github.com/nothings/stb/blob/master/stb_ds.h
