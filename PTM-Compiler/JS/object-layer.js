
class ObjectLayer {
    Cells = [];
    Cols = null;
    Rows = null;
    Size = null;
    Visible = true;

    constructor(cols, rows) {
        if (cols == null || rows == null)
            throw new Error('ObjectLayer constructor requires 2 arguments');

        this.InitCells(cols, rows);
    }

    InitCells(cols, rows) {
        this.Cols = cols;
        this.Rows = rows;
        this.Size = cols * rows;
        
        for (let i = 0; i < this.Size; i++) {
            this.Cells.push(new ObjectCell());
        }
    }

    Fill(o) {
        for (let y = 0; y < this.Rows; y++) {
            for (let x = 0; x < this.Cols; x++) {
                this.SetObject(o, x, y);
            }
        }
    }

    RemoveAllObjects() {
        for (let y = 0; y < this.Rows; y++) {
            for (let x = 0; x < this.Cols; x++) {
                this.RemoveObject(x, y);
            }
        }
    }

    IsWithinBounds(x, y) {
        return x >= 0 && y >= 0 && x < this.Cols && y < this.Rows;
    }

    IsOutOfBounds(x, y) {
        return !this.IsWithinBounds(x, y);
    }

    GetCell(x, y) {
        if (x == null || y == null)
            throw new Error('GetCell requires 2 arguments');
        if (this.IsOutOfBounds(x, y))
            throw new Error(`Object cell X${x} Y${y} is out of bounds`);

        return this.Cells[y * this.Cols + x];
    }

    GetObject(x, y) {
        return this.GetCell(x, y).Object;
    }

    SetObject(o, x, y) {
        const cell = this.GetCell(x, y);
        const replacementObject = o ? o.Copy() : null;
        cell.Object = replacementObject;
    }

    RemoveObject(x, y) {
        this.SetObject(null, x, y);
    }
}
