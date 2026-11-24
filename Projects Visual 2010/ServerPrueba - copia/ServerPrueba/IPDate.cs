using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ServerPrueba
{
    public class IPDate
    {
        public String IP { get; set; }
        public DateTime Date { get; set; }

        //public override String ToString()
        //{
        //    return "IP: "IP + "Fecha: " + Date.ToLongTimeString();
        //}

        //public override bool Equals(object obj)
        //{
        //    if (obj == null) return false;
        //    Part objAsPart = obj as Part;
        //    if (objAsPart == null) return false;
        //    else return Equals(objAsPart);
        //}
        //public override int GetHashCode()
        //{
        //    return PartId;
        //}
        //public bool Equals(Part other)
        //{
        //    if (other == null) return false;
        //    return (this.PartId.Equals(other.PartId));
        //}
        // Should also override == and != operators.
    }
}
