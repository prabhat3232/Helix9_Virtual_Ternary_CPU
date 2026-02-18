
import random

class Matrix:
    """
    Helix Matrix Shim for Python Execution.
    Supports Matrix[R, C] syntax and element-wise operations.
    """
    
    def __class_getitem__(cls, item):
        # Handle Matrix[R, C] type hint syntax
        return cls

    def __init__(self, rows=2, cols=2, fill=None):
        self.rows = rows
        self.cols = cols
        if fill is None:
            self.data = [[random.randint(0, 5) for _ in range(cols)] for _ in range(rows)]
        else:
            self.data = [[fill] * cols for _ in range(rows)]

    def __repr__(self):
        s = f"Matrix({self.rows}x{self.cols}):\n"
        for row in self.data:
            s += "  " + str(row) + "\n"
        return s

    def __matmul__(self, other):
        # Matrix Multiplication @
        if self.cols != other.rows:
            raise ValueError("Dimension mismatch")
        result = Matrix(self.rows, other.cols, fill=0)
        for i in range(self.rows):
            for j in range(other.cols):
                for k in range(self.cols):
                    result.data[i][j] += self.data[i][k] * other.data[k][j]
        return result

    def __add__(self, other):
        # Element-wise Add
        if self.rows != other.rows or self.cols != other.cols:
            raise ValueError("Dimension mismatch")
        result = Matrix(self.rows, self.cols, fill=0)
        for i in range(self.rows):
            for j in range(self.cols):
                result.data[i][j] = self.data[i][j] + other.data[i][j]
        return result

    def __sub__(self, other):
        # Element-wise Sub
        if self.rows != other.rows or self.cols != other.cols:
            raise ValueError("Dimension mismatch")
        result = Matrix(self.rows, self.cols, fill=0)
        for i in range(self.rows):
            for j in range(self.cols):
                result.data[i][j] = self.data[i][j] - other.data[i][j]
        return result
