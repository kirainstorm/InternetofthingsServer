using NewPlatCenter.App_Start;
using System.Web;
using System.Web.Mvc;

namespace NewPlatCenter
{
    public class FilterConfig
    {
        public static void RegisterGlobalFilters(GlobalFilterCollection filters)
        {
            filters.Add(new HandleErrorAttribute());
            filters.Add(new NoCacheAttribute());//不缓存
        }
    }
}
