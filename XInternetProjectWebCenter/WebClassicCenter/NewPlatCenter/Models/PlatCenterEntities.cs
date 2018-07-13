using NewPlatCenter.ViewModels;
using System.Data.Entity;

namespace NewPlatCenter.Models
{
    //值得一提的是，属性为的头上有"[Key]"或"[Required]"的标识，其中[Key]表示在数据库中该字段为主键，[Required]表示不为空，[StringLength]也就是长度了
    //特别注意： 
    //一定要长度，否则soci读不出（NVARCHAR(MAX)）


    //不用关系映射，自己管理数据的添加删除修改
    public class PlatCenterEntities : DbContext
    {
        //---------------------------------------------------------------------------------
        public DbSet<Administrator> Administrators { get; set; }


        //---------------------------------------------------------------------------------
        public DbSet<Account> Accounts { get; set; }
        public DbSet<AccountGroup> AccountGroups { get; set; }
        public DbSet<AccountGroupDevice> AccountGroupDevices { get; set; }

        //---------------------------------------------------------------------------------
        public DbSet<Area> Areas { get; set; }


        //---------------------------------------------------------------------------------
        public DbSet<Server> Servers { get; set; }


        //---------------------------------------------------------------------------------
        public DbSet<Device> Devices { get; set; }


        //---------------------------------------------------------------------------------
        public DbSet<Log> Logs { get; set; }
    }
}