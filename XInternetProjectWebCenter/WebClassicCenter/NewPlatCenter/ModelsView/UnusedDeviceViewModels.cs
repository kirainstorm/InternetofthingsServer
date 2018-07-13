using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Web;

namespace NewPlatCenter.ViewModels
{
    public class UnusedDeviceViewModels
    {
        [Key]
        public int id { get; set; }
        public string text { get; set; }
        public int parent { get; set; }

        public List<UnusedDeviceViewModels> children { get; set; }
    }
}