using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class CommandTranslator
    {
        private string Parse(string src, string cmd, string rest)
        {
            string cpp = "";
            CommandArgument[] args = ParseArgs(rest);
            if (!ValidateArgs(args))
                throw new CompileError("Syntax error: " + src);

            if (cmd == "VAR")
            {
                RequireArgs(src, args, 2);
                Debugger.Break();
            }
            else
            {
                throw new CompileError("Invalid command: " + src);
            }

            return cpp;
        }

        public string Translate(string ptml)
        {
            string cpp = "";
            string cmd = "";
            string argstr = "";

            int ixFirstSpace = ptml.IndexOf(' ');
            if (ixFirstSpace >= 0)
            {
                cmd = ptml.Substring(0, ixFirstSpace).Trim();
                argstr = ptml.Substring(ixFirstSpace + 1).Trim();
            }
            else
            {
                cmd = ptml;
            }

            cpp = Parse(ptml, cmd, argstr);
            return cpp + " // " + ptml;
        }

        private CommandArgument[] ParseArgs(string src)
        {
            List<string> argstr = new List<string>();
            StringBuilder sb = new StringBuilder();
            bool quote = false;

            for (int i = 0; i < src.Length; i++)
            {
                char ch = src[i];

                if (ch == '"')
                {
                    quote = !quote;
                }

                if ((ch != ',' && ch != ';') || quote)
                {
                    sb.Append(ch);
                }

                if (ch == ';' && !quote)
                {
                    argstr.Add(sb.ToString().Trim());
                    break;
                }
                else if ((ch == ',' && !quote) || i == src.Length - 1)
                {
                    argstr.Add(sb.ToString().Trim());
                    sb.Clear();
                }
            }

            List<CommandArgument> args = new List<CommandArgument>();

            foreach (string arg in argstr)
            {
                CommandArgumentType type;

                if (arg.StartsWith("\"") && arg.EndsWith("\""))
                    type = CommandArgumentType.StringLiteral;
                else if (char.IsDigit(arg[0]))
                    type = CommandArgumentType.NumberLiteral;
                else if (char.IsLetter(arg[0]))
                    type = CommandArgumentType.VariableIdentifier;
                else
                    type = CommandArgumentType.Invalid;

                args.Add(new CommandArgument(type, arg));
            }

            return args.ToArray();
        }

        private bool ValidateArgs(CommandArgument[] args)
        {
            return !args.Any(arg => arg.Type == CommandArgumentType.Invalid);
        }

        private void RequireArgs(string src, CommandArgument[] args, int count)
        {
            if (args.Length != count)
                throw new CompileError(string.Format("Expected {0} arguments, got {1}: {2}", count, args.Length, src));
        }
    }
}
