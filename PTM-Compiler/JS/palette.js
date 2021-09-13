
class Palette {
    Colors = [];

    constructor() {
        this.InitDefault();
    }

    Add(rgb) {
        this.Colors.push(new Color(rgb));
    }

    Set(index, rgb) {
        this.Colors[index].RGB = rgb;
    }

    InitDefault() {
        this.Add(0x000000);
        this.Add(0xffffff);
        this.Add(0xe0e0e0);
        this.Add(0xc0c0c0);
        this.Add(0x808080);
        this.Add(0x505050);
        this.Add(0x303030);
        this.Add(0x101010);
        this.Add(0x500000);
        this.Add(0x800000);
        this.Add(0xc00000);
        this.Add(0xff0000);
        this.Add(0xff5050);
        this.Add(0xff8080);
        this.Add(0xffc0c0);
        this.Add(0xffe0e0);
        this.Add(0xc02000);
        this.Add(0xff5000);
        this.Add(0xff8000);
        this.Add(0xffc000);
        this.Add(0xffc050);
        this.Add(0xffc080);
        this.Add(0xffa050);
        this.Add(0x808050);
        this.Add(0x505000);
        this.Add(0x808000);
        this.Add(0xc0c000);
        this.Add(0xffff00);
        this.Add(0xffff50);
        this.Add(0xffff80);
        this.Add(0xffffc0);
        this.Add(0xc0c080);
        this.Add(0x005000);
        this.Add(0x008000);
        this.Add(0x00c000);
        this.Add(0x00ff00);
        this.Add(0x80ff00);
        this.Add(0x80ff80);
        this.Add(0xc0ff00);
        this.Add(0xc0ff80);
        this.Add(0x005050);
        this.Add(0x008080);
        this.Add(0x00c0c0);
        this.Add(0x00ffff);
        this.Add(0x80e0ff);
        this.Add(0x00ffc0);
        this.Add(0x00ff80);
        this.Add(0x00ff50);
        this.Add(0x000050);
        this.Add(0x000080);
        this.Add(0x0000c0);
        this.Add(0x0000ff);
        this.Add(0x0050ff);
        this.Add(0x0080ff);
        this.Add(0x00a0ff);
        this.Add(0x00c0ff);
        this.Add(0x200050);
        this.Add(0x300050);
        this.Add(0x500080);
        this.Add(0x8000ff);
        this.Add(0x8050ff);
        this.Add(0xc080ff);
        this.Add(0xc0a0ff);
        this.Add(0xc0c0ff);
        this.Add(0x500050);
        this.Add(0x800080);
        this.Add(0xc000c0);
        this.Add(0xff00ff);
        this.Add(0xff50ff);
        this.Add(0xff80ff);
        this.Add(0xffc0ff);
        this.Add(0xffe0ff);
    }
}
