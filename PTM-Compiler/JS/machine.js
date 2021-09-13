
class Machine {
    Running = false;
    Display = null;

    constructor() {
        Machine.Info('Machine instance created');
        this.Display = new Display(this);
    }

    Reset() {
        location.reload();
    }

    static Info(obj) {
        let output = '';
        if (typeof obj === 'object') {
            output = JSON.stringify(obj);
        }
        else { 
            output = obj.toString();
        }
        console.log('PTM >> ' + output);
    }

    static Error(error) {
        console.error('PTM >> ' + error);
    }

    static Warning(msg) {
        console.warn('PTM >> ' + msg);
    }

    Run() {
        if (this.Running) {
            Machine.Warning('Main loop is already running');
        }
        else {
            this.Running = true;
            Machine.Info('Main loop started');

            try {
                this.Display.StartRendering();
            }
            catch (error) {
                this.Stop(error);
            }
        }
    }

    Stop(error) {
        this.Running = false;
        if (error) {
            Machine.Error('Main loop stopped abnormally due to:\n' + error.message + '\n' + error.stack);
        }
        else {
            Machine.Info('Main loop stopped normally');
        }
        this.Display.StopRendering();
    }
}
