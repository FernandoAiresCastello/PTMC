using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class PtmlToCppMapping
    {
        public string Ptml { set; get; }
        public string Cpp { set; get; }

        public override string ToString()
        {
            return Ptml + " " + Cpp;
        }
    }

    public class CommandMap
    {
        private readonly string CommandMapFile = Properties.Resources.cmd;
        private bool InsideDefinition = false;
        private string CurCommand = "";
        private string CurArgs = "";

        public Dictionary<string, PtmlToCppMapping> Mappings { get; private set; } = 
            new Dictionary<string, PtmlToCppMapping>();

        public CommandMap()
        {
            List<string> cppLines = new List<string>();

            foreach (string rawLine in CommandMapFile.Split('\n'))
            {
                string line = rawLine.Trim();
                if (string.IsNullOrWhiteSpace(line))
                    continue;

                if (!InsideDefinition)
                {
                    if (line == "[")
                    {
                        InsideDefinition = true;
                        continue;
                    }

                    int ixFirstSpace = line.IndexOf(' ');
                    if (ixFirstSpace > 0)
                    {
                        CurCommand = line.Substring(0, ixFirstSpace).Trim().ToUpper();
                        CurArgs = line.Substring(ixFirstSpace);
                    }
                    else
                    {
                        CurCommand = line;
                        CurArgs = "";
                    }
                }
                else
                {
                    if (line == "]")
                    {
                        InsideDefinition = false;

                        PtmlToCppMapping mapping = new PtmlToCppMapping();
                        mapping.Ptml = CurCommand.Trim() + (CurArgs != "" ? " " + CurArgs.Trim() : "");
                        mapping.Cpp = string.Join(Environment.NewLine, cppLines);
                        Mappings.Add(CurCommand, mapping);

                        cppLines.Clear();
                        continue;
                    }

                    cppLines.Add(line);
                }
            }
        }
    }
}
