
class ObjectMap {
    Layers = [];
    Cols = null;
    Rows = null;

    constructor(layers, cols, rows) {
        if (layers == null || cols == null || rows == null)
            throw new Error('ObjectMap constructor requires 3 arguments');

        this.InitLayers(layers, cols, rows);
    }

    InitLayers(layers, cols, rows) {
        this.Cols = cols;
        this.Rows = rows;
        
        for (let i = 0; i < layers; i++) {
            this.Layers.push(new ObjectLayer(cols, rows));
        }
    }

    RemoveAllObjects() {
        for (let z = 0; z < this.Layers.length; z++) {
            for (let y = 0; y < this.Rows; y++) {
                for (let x = 0; x < this.Cols; x++) {
                    this.Layers[z].RemoveObject(x, y);
                }
            }
        }
    }
}
