
// aie-opt --aie-create-flows --aie-find-flows %s | aie-translate --aie-generate-xaie

module {
  %t70 = AIE.tile(7, 0)
  %t71 = AIE.tile(7, 1)
  %t72 = AIE.tile(7, 2)
  %t73 = AIE.tile(7, 3)
  %t74 = AIE.tile(7, 4)

  // Fixup
  %sw = AIE.switchbox(%t70) {
    AIE.connect<"South" : 3, "North" : 0>
    AIE.connect<"South" : 7, "North" : 1>
    AIE.connect<"North" : 0, "South" : 2>
    AIE.connect<"North" : 1, "South" : 3>
  }
  %mux = AIE.shimmux(%t70) {
    AIE.connect<"DMA" : 0, "South": 3>
    AIE.connect<"DMA" : 1, "South": 7>
    AIE.connect<"South" : 2, "DMA": 0>
    AIE.connect<"South" : 3, "DMA": 1>
  }

  AIE.flow(%t71, "South" : 0, %t72, "DMA" : 0)
  AIE.flow(%t71, "South" : 1, %t74, "DMA" : 0)

  %buf72_0 = AIE.buffer(%t72) : memref<256xi32>
  %buf72_1 = AIE.buffer(%t72) : memref<256xi32>
  %buf74_0 = AIE.buffer(%t74) : memref<256xi32>
  %buf74_1 = AIE.buffer(%t74) : memref<256xi32>

  %l72_0 = AIE.lock(%t72, 0)
  %l72_1 = AIE.lock(%t72, 1)
  %l74_0 = AIE.lock(%t74, 0)
  %l74_1 = AIE.lock(%t74, 1)

  %m72 = AIE.mem(%t72) {
      %srcDma = AIE.dmaStart("S2MM0", ^bd0, ^end)
    ^bd0:
      AIE.useLock(%l72_0, "Acquire", 0, 0)
      AIE.dmaBd(<%buf72_0 : memref<256xi32>, 0, 256>, 0)
      AIE.useLock(%l72_0, "Release", 1, 0)
      br ^bd1
    ^bd1:
      AIE.useLock(%l72_1, "Acquire", 0, 0)
      AIE.dmaBd(<%buf72_1 : memref<256xi32>, 0, 256>, 0)
      AIE.useLock(%l72_1, "Release", 1, 0)
      br ^bd0
    ^end:
      AIE.end
  }

  %m74 = AIE.mem(%t74) {
      %srcDma = AIE.dmaStart("S2MM0", ^bd0, ^end)
    ^bd0:
      AIE.useLock(%l74_0, "Acquire", 0, 0)
      AIE.dmaBd(<%buf74_0 : memref<256xi32>, 0, 256>, 0)
      AIE.useLock(%l74_0, "Release", 1, 0)
      br ^bd1
    ^bd1:
      AIE.useLock(%l74_1, "Acquire", 0, 0)
      AIE.dmaBd(<%buf74_1 : memref<256xi32>, 0, 256>, 0)
      AIE.useLock(%l74_1, "Release", 1, 0)
      br ^bd0
    ^end:
      AIE.end
  }


}