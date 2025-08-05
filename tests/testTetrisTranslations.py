import os
import unittest
from tetrisTranslations import Tetris

class TestTetrisTranslations(unittest.TestCase):
    def assertBoardEqual(self, computedBoardInt: int, expectedBoardInt: int, msg=None):
        if computedBoardInt == expectedBoardInt:
            return

        print("\n" + "="*40)
        print("DEBUGGING INFO: Board states do not match!")
        print("="*40)
        
        differenceBoard = computedBoardInt ^ expectedBoardInt

        tempBoard = Tetris()
        boards = [
            format(computedBoardInt, f'0{tempBoard.tileCount}b'),
            format(expectedBoardInt, f'0{tempBoard.tileCount}b'),
            format(differenceBoard, f'0{tempBoard.tileCount}b'),
        ]
        
        print("\nBOARD STATES:")
        for row in range(0, tempBoard.tileCount, tempBoard.boardWidth):
            print(f"{int(row/10)}".ljust(5, " "), end="")
            start = row
            end   = row + tempBoard.boardWidth
            [print(bitmap[start:end] + " " * 5, end="") for bitmap in boards]
            print()

        self.fail(self._formatMessage(msg, 
            "Board states do not match.")
        )

    def testSingleLineClear(self):
        playfield = int(
            b'0101010101'  \
            b'1111111111'  \
        , 2)

        model         = Tetris(playfield=playfield)
        linesCleared  = model.checkAndClearLines()
        boardState    = model.getBoard()
        finalBoardExpected = int(b'0101010101', 2)

        self.assertEqual(linesCleared, 1, "This should clear only one line")
        self.assertBoardEqual(boardState, finalBoardExpected, "This should be the same line")
    
    def testTetrisClear(self):
        playfield = int(
            b'0010000000'\
            b'0111000000'\
            b'1111111111'\
            b'1111111111'\
            b'1111111111'\
            b'1111111111'\
            b'1111111110'\
        , 2)

        model         = Tetris(playfield=playfield)
        linesCleared  = model.checkAndClearLines()
        boardState    = model.getBoard()
        finalBoardExpected = int(
            b'0010000000'\
            b'0111000000'\
            b'1111110110'\
        , 2)

        self.assertEqual(linesCleared, 4, "This should clear 4 lines")
        self.assertBoardEqual(boardState, finalBoardExpected, "The boards should match")
    

if __name__ == "__main__":
    os.system("cls" if os.name == 'nt' else 'clear')
    print("Running Translated Tetris Tests...")
    unittest.main(verbosity=2)