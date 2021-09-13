
class Display {
    Machine = null;
    Gfx = null;
    FrameCounter = 0;
    FrameRendererHandle = null;
    MapViews = [];

    constructor(machine) {
        if (machine == null)
            throw new Error('Display must have a reference to the machine');

        this.Machine = machine;
        this.Gfx = new GraphicsDriver();
    }

    ClearBackground(backColor) {
        if (backColor != null) {
            this.Gfx.BackColor = backColor;
        }
        this.Gfx.ClearBackground();
    }

    StartRendering() {
        this.FrameCounter = 0;
        this.RenderFrame();
    }

    StopRendering() {
        window.cancelAnimationFrame(this.FrameRendererHandle);
    }

    RenderFrame() {
        try {
            for (let i = 0; i < this.MapViews.length; i++) {
                this.MapViews[i].Render();
            }
            this.FrameCounter++;
            this.FrameRendererHandle = window.requestAnimationFrame(() => this.RenderFrame());
        }
        catch (error) {
            this.Machine.Stop(error);
        }
    }

    AddMapView(view, grid) {
        view.Display = this;
        view.ObjectMap = grid;
        this.MapViews.push(view);
    }
}
