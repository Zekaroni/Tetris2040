class TetrisTester:
    def __init__(self):
        self.playfield = int((b'' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'0000000000' \
            b'1111111111' \
            b'1111111110' \
            b'1111111111' \
            b'1111111111' \
            b'1111111001' \
            b'1111111001'), 2)
        
        self.boardHeight = 20
        self.boardWidth  = 10
        self.tileCount   = self.boardHeight * self.boardWidth

    def updateRows(self, endRow, concurrentRows):
        buffer = self.playfield
        upper_rows = self.boardHeight - endRow - 1 + concurrentRows
        upper_mask = buffer >> (upper_rows * self.boardWidth)
        fullBuffer = (1 << self.tileCount) - 1
        lower_mask = fullBuffer >> ((endRow + 1) * self.boardWidth)
        finalBoard = (
            (buffer & lower_mask) |
            (upper_mask << ((self.boardHeight - endRow - 1) * self.boardWidth))
        )

        # Logging boards
        bitmaps = [
            format(self.playfield, f'0{self.tileCount}b'),
            format(lower_mask, f'0{self.tileCount}b'),
            format(upper_mask, f'0{self.tileCount}b'),
            format(finalBoard, f'0{self.tileCount}b'),
        ]
        for row in range(0, self.tileCount, self.boardWidth):
            start = row
            end   = row + self.boardWidth
            [print(bitmap[start:end] + " " * 5, end="") for bitmap in bitmaps]
            print()
        
        self.playfield = finalBoard

if __name__ == "__main__":
    tetris = TetrisTester()
    tetris.updateRows(17, 2)
    print()
    tetris.updateRows(16, 1)