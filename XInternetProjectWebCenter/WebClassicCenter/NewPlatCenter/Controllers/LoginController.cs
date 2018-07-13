using System;
using System.Collections.Generic;
using System.Data;
using System.Data.Entity;
using System.Linq;
using System.Net;
using System.Web;
using System.Web.Mvc;
using NewPlatCenter.Models;
using NewPlatCenter.ViewModels;

namespace NewPlatCenter.Views
{
    //使用Administrators表登陆后台，并且当没有数据库时初始化数据库
    public class LoginController : Controller
    {
        private PlatCenterEntities db = new PlatCenterEntities();

        public int InitDatabase()
        {
            List<Administrator> list = db.Administrators.ToList();
            if (list.Count == 0)
            {
                //---------------------------------------------初始化超级管理员
                Administrator _admin = new Administrator()
                {
                    Admin_Name = "administrator",
                    Admin_Pwd = "123456",
                };
                db.Administrators.Add(_admin);
                //---------------------------------------------新增根区域
                Area _area = new Area()
                {
                    Ar_Name = "默认区域",
                    Ar_Admin = 1,
                };
                db.Areas.Add(_area);


                //---------------------------------------------新增根区域
                AccountGroup _acgp = new AccountGroup()
                {
                    Ag_Name = "默认列表",
                };
                db.AccountGroups.Add(_acgp);
                //---------------------------------------------更新表
                db.SaveChanges();
            }
            return 0;
        }
        public ActionResult Index()
        {
            InitDatabase();//初始化数据库

            if (Session["userID"] != null)
            {
                if (Session["userName"].ToString().Equals("administrator"))
                    return RedirectToAction("Index", "Home");
            }
            return View();
        }
        [HttpPost]
        public JsonResult Login(LogOnModel logon, string returnUrl)
        {
            List<Administrator> list = (from m in db.Administrators
                                        where ((m.Admin_Name == logon.UserName) && (m.Admin_Pwd == logon.Password))
                                        select m).ToList();
            if (0 == list.Count())
            {
                return Json("请检查用户名或者密码!", JsonRequestBehavior.AllowGet);
            }
            else
            {
                Administrator _user = list[0];
                Session["userID"] = _user.ID;
                Session["userName"] = _user.Admin_Name;
            }
            return Json("sus", JsonRequestBehavior.AllowGet);
        }
        public ActionResult Logout()
        {
            //var data = db.Users.ToList();
            if (Session["userID"] != null)
            {
                Session["userID"] = null;
                Session["userName"] = null;
            }
            return RedirectToAction("Index", "Account");
        }
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                db.Dispose();
            }
            base.Dispose(disposing);
        }
    }
}
