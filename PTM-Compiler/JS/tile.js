
class Tile {
    Index = 0;
    ForeColor = 0;
    BackColor = 0;

    constructor(ix, fgc, bgc) {
        if (ix == null || fgc == null || bgc == null )
            throw new Error('Tile constructor requires 3 parameters');

        this.Index = ix;
        this.ForeColor = fgc;
        this.BackColor = bgc;
    }

    Copy() {
        return new Tile(this.Index, this.ForeColor, this.BackColor);
    }
}
