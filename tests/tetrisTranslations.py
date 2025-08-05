import os

class Tetris:
    def __init__(self, playfield: int = 0, debug: bool = False):
        self.playfield = playfield
        self.boardHeight   = 20
        self.boardWidth    = 10
        self.tileCount     =  self.boardHeight * self.boardWidth
        self.rowMask       = (1 << self.boardWidth) - 1
        self.fullBoardMask = int(b'1' * self.tileCount, 2)

        self.debug = debug

    def getRow(self, rowIndex) -> int:
        shift = (self.boardHeight - rowIndex - 1) * self.boardWidth
        mask = (1 << self.boardWidth) - 1
        return (self.playfield >> shift) & mask

    def checkAndClearLines(self) -> bool:
        linesCleared = 0
        concurrentLines = 0
        row = self.boardHeight - 1

        while (row >= 0):
            if self.getRow(row) == self.rowMask:
                concurrentLines += 1
            else:
                if concurrentLines:
                    self.updateRows(row, concurrentLines)
                    linesCleared += concurrentLines

                    if self.debug:
                        print("Cleared line(s): " +
                            f"{row+1 if not concurrentLines > 1 else f'{row+1} -> {row+concurrentLines}'}"
                        )

                    row += concurrentLines 
                    concurrentLines = 0

            if self.debug:
                print(
                    f"{row}".ljust(5) + "{:08b}".format(self.getRow(row)) +
                    f"     CL={concurrentLines}"
                )
            
            row-=1

        return linesCleared

    def updateRows(self, startRow, concurrentRows):
        buffer = self.playfield # This will be not be needed in C++, use actual instead of buffer

        shiftUpper = (self.boardHeight - startRow - 1) * self.boardWidth
        shiftLower = (startRow + concurrentRows + 1) * self.boardWidth
        shiftFinal = (self.boardHeight - startRow - concurrentRows - 1) * self.boardWidth

        upperMask = buffer >> shiftUpper
        lowerMask = self.fullBoardMask >> shiftLower
        
        finalBoard = ((buffer & lowerMask) | (upperMask << shiftFinal))

        if self.debug:
            # Logging boards
            bitmaps = [
                format(self.playfield, f'0{self.tileCount}b'),
                format(upperMask, f'0{self.tileCount}b'),
                format(lowerMask, f'0{self.tileCount}b'),
                format(finalBoard, f'0{self.tileCount}b'),
            ]
            names = [
                "Current",
                "Upper",
                "Lower",
                "Final"
            ]
            print("     ", end="")
            for i in range(len(names)):
                print(f"{names[i]}".ljust(10, " ") + " "*5, end="")
            print()

            for row in range(0, self.tileCount, self.boardWidth):
                print(f"{int(row/10)}".ljust(5, " "), end="")
                start = row
                end   = row + self.boardWidth
                [print(bitmap[start:end] + " " * 5, end="") for bitmap in bitmaps]
                print()
            
        self.playfield = finalBoard

    def printBoard(self):
        board =format(self.playfield, f'0{self.tileCount}b')
        for position in range(0, self.tileCount, self.boardWidth):
            start = position
            end   = position + self.boardWidth
            print(board[start:end])
    
    def getBoard(self) -> int:
        """
        return: an int representation of the board
        """
        return int(self.playfield)

if __name__ == "__main__":
    os.system("cls")
    tetris = Tetris()
    tetris.checkAndClearLines()
    print(tetris.getBoard())