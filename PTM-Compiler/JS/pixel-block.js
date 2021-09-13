
class PixelBlock {
    Width = 8;
    Height = 8;
    Length = this.Width * this.Height;
    Pixels = null;

    constructor(row0, row1, row2, row3, row4, row5, row6, row7) {
        this.Set(row0, row1, row2, row3, row4, row5, row6, row7);
    }

    Set(row0, row1, row2, row3, row4, row5, row6, row7) {
        this.Pixels = '';
        this.Pixels += Util.ByteToBinaryString(row0);
        this.Pixels += Util.ByteToBinaryString(row1);
        this.Pixels += Util.ByteToBinaryString(row2);
        this.Pixels += Util.ByteToBinaryString(row3);
        this.Pixels += Util.ByteToBinaryString(row4);
        this.Pixels += Util.ByteToBinaryString(row5);
        this.Pixels += Util.ByteToBinaryString(row6);
        this.Pixels += Util.ByteToBinaryString(row7);
    }

    SetBlank() {
        this.Set(0, 0, 0, 0, 0, 0, 0, 0);
    }
}
