//  SimpleStoreErp
//	Copyright(C) 2021 殷越
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program. If not, see <https://www.gnu.org/licenses/>.
#include "common.h"
#include "MainWindow.h"
#include "EditItemWindow.h"
#include "ProductRecord.h"
#include "UserSetting.h"
#include "LoadDataFilterWindow.h"
#include "LoginWindow.h"
#include "InputBoxWindow.h"
#include <CommCtrl.h>
#define MAIN_DISPLAYPAGESIZE 10
#define MAIN_STATUSBAR_COM 4
#define ID_MENU_ABOUT 1
#define ID_MENU_VWS 2
#define ID_MENU_ADDRECORD 3
#define ID_LISTVIEW_MAIN 4
#define ID_BUTTON_PREVPAGE 5
#define ID_BUTTON_NEXTPAGE 6
#define ID_EDIT_PAGE 7
#define ID_CHECKBOX_PAGE 8
#define ID_MENU_SAVE 9
#define ID_MENU_LOADALL 10
#define ID_MENU_FLITER 11
#define ID_MENU_FLITERLOADALL 12
#define ID_STATUSBAR_MAIN 13
#define ID_BUTTON_REMOVESELECTEDITEMS 14
#define ID_MENU_REMOVEUSER 15
#define ID_MENU_ADDUSER 16
#define ID_MENU_CHANGEPERR 17
#define ID_MENU_CHANGEPWD 18
#define ID_MENU_SHOWUSERSLIST 19
#define ID_MENU_IMPORT 20
#define ID_MENU_CALCMAINPROVIDER 21
#define ID_MENU_CALCMAINRECIVER 22
#define ID_MENU_CALCMAINRESENTER 23
#define ID_MENU_CALCMAINSIGNER 24
#define ID_MENU_CALCALLCASH 25
#define ID_MENU_CALCALLCOST 26

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void configstatusbar(HWND hwndParent,HWND  hwndStatus)
{
    int cParts = MAIN_STATUSBAR_COM;
    RECT rcClient;

    // Get the coordinates of the parent window's client area.
    GetClientRect(hwndParent, &rcClient);

    // Allocate an array for holding the right edge coordinates.
    HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * cParts);
    if (hloc)
    {
        PINT paParts = (PINT)LocalLock(hloc);
        if (paParts)
        {
            // Calculate the right edge coordinate for each part, and
            // copy the coordinates to the array.
            int nWidth = rcClient.right / cParts;
            int rightEdge = nWidth;
            for (int i = 0; i < cParts; i++) {
                paParts[i] = rightEdge;
                rightEdge += nWidth;
            }

            // Tell the status bar to create the window parts.
            SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)
                paParts);

            // Free the array, and return.
            LocalUnlock(hloc);
        }
        else
        {
            UnrecoveryableFailed();
        }

        LocalFree(hloc);
    }
    else 
    {
        UnrecoveryableFailed();
    }
}
//
//   FUNCTION: OnStatusbarSize(HWND, UINT, int, int)
//
//   PURPOSE: Process the WM_SIZE message
//
void OnStatusbarSize(HWND hWnd, UINT state, int cx, int cy)
{
    HWND hStatusbar = GetDlgItem(hWnd, ID_STATUSBAR_MAIN);

    configstatusbar(hWnd, hStatusbar);

    // Resize statusbar so it's always same width as parent's client area
    SendMessage(hStatusbar, WM_SIZE, 0, 0);
}
// Description: 
//   Creates a status bar and divides it into the specified number of parts.
// Parameters:
//   hwndParent - parent window for the status bar.
//   idStatus - child window identifier of the status bar.
//   hinst - handle to the application instance.
//   cParts - number of parts into which to divide the status bar.
// Returns:
//   The handle to the status bar.
//
HWND DoCreateStatusBar(HWND hwndParent, int idStatus, HINSTANCE
    hinst, int cParts)
{
    //see https://docs.microsoft.com/en-us/windows/win32/controls/create-status-bars

    HWND hwndStatus;

    // Ensure that the common control DLL is loaded.
    //InitCommonControls();

    // Create the status bar.
    hwndStatus = CreateWindowEx(
        0,                       // no extended styles
        STATUSCLASSNAME,         // name of status bar class
        (PCTSTR)NULL,           // no text when first created
        SBARS_SIZEGRIP |         // includes a sizing grip
        WS_CHILD | WS_VISIBLE,   // creates a visible child window
        0, 0, 0, 0,              // ignores size and position
        hwndParent,              // handle to parent window
        (HMENU)idStatus,       // child window identifier
        hinst,                   // handle to application instance
        NULL);                   // no window creation data

    
    configstatusbar(hwndParent, hwndStatus);

    return hwndStatus;
}
void CreateMainWindow()
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"yinyue200.SimpleStoreErp.MainWindow";

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = yinyue200_hInstance;
    wc.lpszClassName = CLASS_NAME;

    WORD result = RegisterClass(&wc);
    //printf("%d", result);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"我的小店出入库管理系统",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        yinyue200_hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        int a = GetLastError();
        return 0;
    }
    DoCreateStatusBar(hwnd, ID_STATUSBAR_MAIN, yinyue200_hInstance, 4);
    ShowWindow(hwnd, yinyue200_nCmdShow);


    return 0;
}
void edititemlogined(void* context)
{
    CreateEditItemWindow(context, yinyue200_LoganUserInfo==NULL?false: wcscmp(yinyue200_LoganUserInfo->Type , L"ADMIN")==0);
}
#define LISTVIEWNOTIFTLOADCOLINT(caseid,membername) case caseid:\
{\
    swprintf_s(szString, _countof(szString),\
        TEXT("%lld"),\
        record->##membername);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLDOUBLE(caseid,membername) case caseid:\
{\
    swprintf_s(szString, _countof(szString),\
        TEXT("%lf"),\
        record->##membername);\
    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,\
        szString, _TRUNCATE);\
    break;\
}
#define LISTVIEWNOTIFTLOADCOLWSTR(caseid,membername) case caseid:\
{\
    if(record->##membername!=NULL)\
        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,record->##membername, _TRUNCATE);\
    else\
        wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,L"", _TRUNCATE);\
    break;\
}
LRESULT ListViewNotify(HWND hWnd, LPARAM lParam)
{
    LPNMHDR  lpnmh = (LPNMHDR)lParam;
    HWND     hwndListView = GetDlgItem(hWnd, ID_LISTVIEW_MAIN);
    YINYUE200_MAINWINDOWDATA* windata = GetProp(hWnd, YINYUE200_WINDOW_DATA);
    if (windata==NULL)
    {
        return 0;
    }
    switch (lpnmh->code)
    {
    case LVN_GETDISPINFO:
    {
        LV_DISPINFO* lpdi = (LV_DISPINFO*)lParam;
        TCHAR szString[MAX_PATH];
        PRODUCTRECORD* record = VECTOR_GET(windata->PagedNowList, PRODUCTRECORD*, lpdi->item.iItem);
        if (record)
        {
            if (lpdi->item.mask & LVIF_TEXT)
            {
                switch (lpdi->item.iSubItem)
                {
                    LISTVIEWNOTIFTLOADCOLWSTR(0, Name)
                    LISTVIEWNOTIFTLOADCOLINT(1,ID)
                    LISTVIEWNOTIFTLOADCOLWSTR(2, Type)
                    LISTVIEWNOTIFTLOADCOLWSTR(3, State)
                    LISTVIEWNOTIFTLOADCOLINT(4, Date)
                    LISTVIEWNOTIFTLOADCOLWSTR(5, ProvideBy)
                    LISTVIEWNOTIFTLOADCOLWSTR(6, RecievedBy)
                    LISTVIEWNOTIFTLOADCOLWSTR(7, ResentBy)
                    LISTVIEWNOTIFTLOADCOLINT(8, Count)
                    LISTVIEWNOTIFTLOADCOLDOUBLE(9, Cost)
                    LISTVIEWNOTIFTLOADCOLDOUBLE(10, Price)
                    LISTVIEWNOTIFTLOADCOLDOUBLE(11, ResentPrice)
                    LISTVIEWNOTIFTLOADCOLWSTR(12, Signer)

                default:
                {
                    swprintf_s(szString, _countof(szString),
                        TEXT("项目 %d - Column %d"),
                        lpdi->item.iItem + 1, lpdi->item.iSubItem);
                    wcsncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
                        szString, _TRUNCATE);
                }
                }

            }
        }   
    }
    return 0;

    case LVN_ODCACHEHINT:
    {
        LPNMLVCACHEHINT   lpCacheHint = (LPNMLVCACHEHINT)lParam;
        /*
        This sample doesn't use this notification, but this is sent when the
        ListView is about to ask for a range of items. On this notification,
        you should load the specified items into your local cache. It is still
        possible to get an LVN_GETDISPINFO for an item that has not been cached,
        therefore, your application must take into account the chance of this
        occurring.
        */
    }
    return 0;

    case LVN_ODFINDITEM:
    {
        LPNMLVFINDITEM lpFindItem = (LPNMLVFINDITEM)lParam;
        /*
        This sample doesn't use this notification, but this is sent when the
        ListView needs a particular item. Return -1 if the item is not found.
        */
    }
    return 0;
    case LVN_COLUMNCLICK:
    {
        LPNMLISTVIEW info = lParam;
        YINYUE200_MAINWINDOWDATA* windata = GetProp(hWnd, YINYUE200_WINDOW_DATA);
        if (windata->sortcomindex == info->iSubItem)
        {
            windata->sortmethod++;
            if (windata->sortmethod > 2)
            {
                windata->sortmethod = 0;
                windata->sortcomindex = -1;
            }
        }
        else
        {
            windata->sortcomindex = info->iSubItem;
            windata->sortmethod = 1;
        }
        windata->sortstate = 0;
        Yinyue200_Main_UpdateListViewData(hWnd);
    }
    case NM_DBLCLK:
    {
        LPNMITEMACTIVATE lpnmitem = lParam;
        if (lpnmitem->iItem >= 0)
        {
            CreateLoginWindow(GetNowLoginedUserName(), edititemlogined, VECTOR_GET(windata->PagedNowList, PRODUCTRECORD_PTR, lpnmitem->iItem));
        }
        break;
    }
    }

    return 0;
}
HWND Yinyue200_Main_CreateListView(HWND hwndParent)
{
    DWORD       dwStyle;
    HWND        hwndListView;
    BOOL        bSuccess = TRUE;

    dwStyle = WS_TABSTOP |
        WS_CHILD |
        WS_BORDER |
        WS_VISIBLE |
        LVS_AUTOARRANGE |
        LVS_REPORT |
        LVS_OWNERDATA;

    hwndListView = CreateWindowEx(WS_EX_CLIENTEDGE,          // ex style
        WC_LISTVIEW,               // class name - defined in commctrl.h
        TEXT(""),                        // dummy text
        dwStyle,                   // style
        0,                         // x position
        0,                         // y position
        1000,                         // width
        500,                         // height
        hwndParent,                // parent
        (HMENU)ID_LISTVIEW_MAIN,        // ID
        (HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE),  // instance
        NULL);                     // no extra data

    if (!hwndListView)
        return NULL;

    return hwndListView;
}
BOOL InsertListViewItems(HWND hwndListView,size_t count)
{
    //empty the list
    ListView_DeleteAllItems(hwndListView);

    //set the number of items in the list
    ListView_SetItemCount(hwndListView, count);

    return TRUE;
}
#define DEFINE_NAMEANDTHEIRDISPLAYSORTORDER(name){ TEXT(name) ,TEXT(name) L" ↑",TEXT(name) L" ↓" }
#define MAINWINDOW_COLUMNCOUNT 13
void Yinyue200_Main_SetListViewColumn(HWND hwnd,BOOL first)
{
    HWND hwndListView = GetDlgItem(hwnd, ID_LISTVIEW_MAIN);
    LPWSTR       szString_o[MAINWINDOW_COLUMNCOUNT][3] = {
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("名称"), 
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("ID"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("类型"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("状态"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("日期"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("供货商"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("收货商"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("退货商"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("数量"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("进价"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("销售价"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("退货价"),
        DEFINE_NAMEANDTHEIRDISPLAYSORTORDER("经手人"),
    };//二维数组，存储不同列在不同排序状态下的列头字符串
    LV_COLUMN   lvColumn;
    int         i;
    LPWSTR       szString[MAINWINDOW_COLUMNCOUNT];

    YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    int selecol = -1;
    int mode = 0;
    if (windata)
    {
        selecol = windata->sortcomindex;
        if (selecol >= 0)
        {
            mode = windata->sortmethod;
        }
    }

    for (int i = 0; i < MAINWINDOW_COLUMNCOUNT; i++)
    {
        szString[i] = szString_o[i][i == selecol ? mode : 0];
    }

    //empty the list
    if(first)
        ListView_DeleteAllItems(hwndListView);

    //initialize the columns
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvColumn.fmt = LVCFMT_LEFT;
    lvColumn.cx = 120;
    for (i = 0; i < MAINWINDOW_COLUMNCOUNT; i++)
    {
        lvColumn.pszText = szString[i];
        if(first)
            ListView_InsertColumn(hwndListView, i, &lvColumn);
        else
            ListView_SetColumn(hwndListView, i, &lvColumn);
    }
}
BOOL Yinyue200_Main_InitListView(HWND hwnd,HWND hwndListView)
{
    Yinyue200_Main_SetListViewColumn(hwnd, true);

    InsertListViewItems(hwndListView, 0);

    return TRUE;
}
typedef struct Yinyue200_Main_ListViewSortContext
{
    //获取排序成员
    void* (*GetCompareObject)(void* obj);
    //是否反转比较结果
    BOOL IS_REV_RESULT;
} YINYUE200_MAINLISTVIEWSORTCONTEXT;
int Yinyue200_Main_UpdateListViewData_PWSTRCompare(void* pcontext, void const* left, void const* right)
{
    //宽字符串排序比较函数
    YINYUE200_MAINLISTVIEWSORTCONTEXT* context = pcontext;
    PRODUCTRECORD_PTR* leftrecord = left;
    PRODUCTRECORD_PTR* rightrecord = right;
    LPWSTR leftstr = context->GetCompareObject(*leftrecord);
    LPWSTR rightstr = context->GetCompareObject(*rightrecord);
    int result = wcscmp(leftstr, rightstr);
    if (context->IS_REV_RESULT)
        return -result;
    else
        return result;
}
int Yinyue200_Main_UpdateListViewData_int64Compare(void* pcontext, void const* left, void const* right)
{
    //整型排序比较函数
    PRODUCTRECORD_PTR* leftrecord = left;
    PRODUCTRECORD_PTR* rightrecord = right;
    YINYUE200_MAINLISTVIEWSORTCONTEXT* context = pcontext;
    int64_t const* l = context->GetCompareObject(*leftrecord);
    int64_t const* r = context->GetCompareObject(*rightrecord);
    int result = *l - *r;
    if (context->IS_REV_RESULT)
        return -result;
    else
        return result;
}
int Yinyue200_Main_UpdateListViewData_doubleCompare(void* pcontext, void const* left, void const* right)
{
    //浮点排序比较函数
    PRODUCTRECORD_PTR* leftrecord = left;
    PRODUCTRECORD_PTR* rightrecord = right;
    YINYUE200_MAINLISTVIEWSORTCONTEXT* context = pcontext;
    double const* l = context->GetCompareObject(*leftrecord);
    double const* r = context->GetCompareObject(*rightrecord);
    int result = *l - *r;
    if (context->IS_REV_RESULT)
        return -result;
    else
        return result;
}
#define ITEM_COMPAREIMPL(casenum,comparetail,name)  case casenum: \
{\
    qsortcontext.GetCompareObject = yinyue200_GetProductRecord##name;\
    vector_qsort(&windata->NowList, Yinyue200_Main_UpdateListViewData_##comparetail##Compare, &qsortcontext);\
    break;\
}
void Yinyue200_Main_UpdateListViewData(HWND hwnd)
{
    HWND lastpagebtn = GetDlgItem(hwnd, ID_BUTTON_PREVPAGE);
    HWND nextpagebtn = GetDlgItem(hwnd, ID_BUTTON_NEXTPAGE);
    HWND editpagebtn = GetDlgItem(hwnd, ID_EDIT_PAGE);
    HWND listview = GetDlgItem(hwnd, ID_LISTVIEW_MAIN);
    YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    if (windata)
    {
        YINYUE200_MAINLISTVIEWSORTCONTEXT qsortcontext = { 0 };
        qsortcontext.IS_REV_RESULT = windata->sortmethod == 2;
        if (windata->sortstate == 0)
        {
            windata->sortstate = 1;
            vector nn = vector_clone(&windata->UnsortedNowList);
            VECTOR_FREE(windata->NowList);
            VECTOR_MOVE(windata->NowList, nn);
            switch (windata->sortcomindex)
            {
            case -1:
            {
                //do nothing
                break;
            }
                //以下是不同字段排序代码实现
                ITEM_COMPAREIMPL(0,PWSTR,Name)
                ITEM_COMPAREIMPL(1,int64,ID)
                ITEM_COMPAREIMPL(2, PWSTR, Type)
                ITEM_COMPAREIMPL(3, PWSTR, State)
                ITEM_COMPAREIMPL(4, int64, Date)
                ITEM_COMPAREIMPL(5, PWSTR, ProvideBy)
                ITEM_COMPAREIMPL(6, PWSTR, RecievedBy)
                ITEM_COMPAREIMPL(7, PWSTR, ResentBy)
                ITEM_COMPAREIMPL(8, int64, Count)
                ITEM_COMPAREIMPL(9, double, Cost)
                ITEM_COMPAREIMPL(10, double, Price)
                ITEM_COMPAREIMPL(11, double, ResentPrice)
                ITEM_COMPAREIMPL(12, PWSTR, Signer)

            default:
                break;
            }
        }
        if (IsDlgButtonChecked(hwnd, ID_CHECKBOX_PAGE))
        {
            //分页显示相关代码
        RESETPAGE:;
            VECTOR_CLEAR(windata->PagedNowList);
            for (size_t i = windata->pagestart; i < VECTOR_TOTAL(windata->NowList)&&i< windata->pagestart+ MAIN_DISPLAYPAGESIZE; i++)
            {
                VECTOR_ADD(windata->PagedNowList, vector_get(&windata->NowList, i));
            }
            if (VECTOR_TOTAL(windata->PagedNowList) == 0)
            {
                windata->pagestart = 0;
                if (VECTOR_TOTAL(windata->NowList) > 0)
                {
                    goto RESETPAGE;
                }
            }
        }
        else
        {
            vector nn = vector_clone(&windata->NowList);
            VECTOR_FREE(windata->PagedNowList);
            VECTOR_MOVE(windata->PagedNowList, nn);
        }
        Yinyue200_Main_SetListViewColumn(hwnd, false);
        InsertListViewItems(listview, VECTOR_TOTAL(windata->PagedNowList));

        wchar_t message[30];
        swprintf(message, 30, L"已加载 %d 项", (int)(VECTOR_TOTAL(windata->NowList)));
        SendMessage(GetDlgItem(hwnd, ID_STATUSBAR_MAIN), SB_SETTEXT, MAKEWPARAM(0, 0), message);//更新状态栏
    }
}
//更新按钮状态并更新主列表
void UpdateCheckBoxInfo(HWND hwnd,YINYUE200_MAINWINDOWDATA* windowdata)
{
    Yinyue200_Main_UpdateListViewData(hwnd);
    HWND lastpagebtn = GetDlgItem(hwnd, ID_BUTTON_PREVPAGE);
    HWND nextpagebtn = GetDlgItem(hwnd, ID_BUTTON_NEXTPAGE);
    HWND editpagebtn = GetDlgItem(hwnd, ID_EDIT_PAGE);
    if (IsDlgButtonChecked(hwnd, ID_CHECKBOX_PAGE))
    {
        ShowWindow(lastpagebtn, SW_SHOW);
        ShowWindow(nextpagebtn, SW_SHOW);
        ShowWindow(editpagebtn, SW_SHOW);
        if (windowdata->pagestart + VECTOR_TOTAL(windowdata->PagedNowList) >= VECTOR_TOTAL(windowdata->NowList))
        {
            EnableWindow(nextpagebtn, FALSE);
        }
        else
        {
            EnableWindow(nextpagebtn, TRUE);
        }
        EnableWindow(lastpagebtn, windowdata->pagestart > 0);
        wchar_t buf[30];
        swprintf(buf, 30, L"%d", windowdata->pagestart / MAIN_DISPLAYPAGESIZE + 1);
        LPWSTR editpagestr = CreateWstrForWindowText(editpagebtn);
        if (wcscmp(editpagestr, buf) != 0)//只有当页码改变时才发送消息
        {
            SendMessage(editpagebtn, WM_SETTEXT, 0, buf);
        }
        free(editpagestr);
    }
    else
    {
        //分页显示复选框关闭时，隐藏相关控件
        ShowWindow(lastpagebtn, SW_HIDE);
        ShowWindow(nextpagebtn, SW_HIDE);
        ShowWindow(editpagebtn, SW_HIDE);
    }
}
void yinyue200_main_loadnowlist(HWND hwnd,YINYUE200_MAINWINDOWDATA *windata)
{
    vector t = vector_clone(&yinyue200_ProductList);;
    VECTOR_MOVE(windata->UnsortedNowList, t);
    windata->sortstate = 0;//设置排序状态为未排序，强制重新排序

    UpdateCheckBoxInfo(hwnd, windata);
}
typedef struct yinyue200_action_addusercontext
{
    HWND hwnd;
    LPWSTR Username;
    LPWSTR Password;
} YINYUE200_ACTION_ADDUSERCONTEXT; 
void adduserpwdinputedrepeat(void* context, LPWSTR userpwd)
{
    //新增用户确认密码输入完成回调

    YINYUE200_ACTION_ADDUSERCONTEXT* con = context;
    EnableWindow(con->hwnd, true);
    if (userpwd != NULL)
    {
        if (wcscmp(userpwd, con->Password) != 0)
        {
            MessageBox(con->hwnd, L"两次密码不一致", NULL, 0);
            free(con->Password);
            free(con->Username);
            return;
        }
        bool ISADMIN;
        int msgboxret = MessageBox(con->hwnd, L"该用户是否要设置为管理员用户", L"创建用户", MB_YESNOCANCEL | MB_ICONQUESTION);
        switch (msgboxret)
        {
        case IDYES:
        {
            ISADMIN = true;
            break;
        }
        case IDNO:
        {
            ISADMIN = false;
            break;
        }
        default:
            free(con->Password);
            free(con->Username);
            return;
        }
        wchar_t pwdhash[65];
        Hash256LPWSTR(con->Password, pwdhash);
        free(con->Password);
        vector* vec = UserRecordLoadToVector(yinyue200_GetUserConfigFilePath());
        bool added = false;
        LPWSTR adminstr = L"ADMIN";
        LPWSTR normalstr = L"NORMAL";
        for (size_t i = 0; i < vector_total(vec); i++)
        {
            USERDATAINFO_PTR one = vector_get(vec, i);
            if (wcscmp(one->Name, con->Username) == 0)
            {
                //如果用户已存在则修改已有用户而不是创建新用户
                free(one->PasswordHash);
                free(one->Type);
                one->PasswordHash = pwdhash;
                one->Type = ISADMIN ? adminstr : normalstr;
                goto USERINFOADDEND;
            }
        }

        USERDATAINFO* userinfo = yinyue200_safemalloc(sizeof(USERDATAINFO));
        added = true;  
        userinfo->Name = con->Username;
        userinfo->PasswordHash = pwdhash;
        userinfo->Type = ISADMIN ? adminstr : normalstr;
        vector_add(vec, userinfo);
    USERINFOADDEND:;
        yinyue200_UserRecordSaveToFile(yinyue200_GetUserConfigFilePath(), vec);

        if (added)
        {
            free(userinfo);
        }
        size_t vv = added ? vector_total(vec) - 1 : vector_total(vec);
        for (size_t i = 0; i < vv; i++)
        {
            USERDATAINFO_PTR one = vector_get(vec, i);
            free(one->Name);
            if (one->PasswordHash != pwdhash)//防止释放不在堆中的内容
            {
                free(one->PasswordHash);
            }
            if (one->Type != adminstr && one->Type != normalstr)//防止释放不在堆中的内容
            {
                free(one->Type);
            }
            free(one);
        }

        
        free(con->Username);
        vector_free(vec);
        free(vec);
    }
    else
    {
        //用户按了取消按钮
        free(con->Password);
        free(con->Username);
    }
}
void adduserpwdinputed(void* context, LPWSTR userpwd)
{
    //新增用户密码输入完成回调

    YINYUE200_ACTION_ADDUSERCONTEXT* con = context;
    if (userpwd != NULL)
    {
        con->Password = userpwd;
        CreateInputBoxWindow(L"再次输入密码", adduserpwdinputedrepeat, context, true);
    }
    else
    {
        EnableWindow(con->hwnd, true);
        free(con->Username);
        free(con);
    }
}
void addusernameinputed(void* context, LPWSTR username)
{
    //添加用户用户名输入完成回调

    if (username != NULL)
    {
        YINYUE200_ACTION_ADDUSERCONTEXT* context_next = yinyue200_safemalloc(sizeof(YINYUE200_ACTION_ADDUSERCONTEXT));
        memset(context_next, 0, sizeof(YINYUE200_ACTION_ADDUSERCONTEXT));
        context_next->hwnd = context;
        context_next->Username = username;
        CreateInputBoxWindow(L"输入密码", adduserpwdinputed, context_next,true);
    }
    else
    {
        //用户按了取消按钮
        EnableWindow(context, true);
    }
}
void removeusernameinputed(void* context, LPWSTR username)
{
    //删除用户用户名输入完成回调

    if (username != NULL)
    {
        vector* vec = UserRecordLoadToVector(yinyue200_GetUserConfigFilePath());
        for (size_t i = 0; i < vector_total(vec); i++)
        {
            USERDATAINFO_PTR one = vector_get(vec, i);
            if (wcscmp(one->Name, username)==0)
            {
                vector_delete(vec, i);
                free(one->Name);
                free(one->PasswordHash);
                free(one->Type);
                free(one);
                i--;
            }

        }
        yinyue200_UserRecordSaveToFile(yinyue200_GetUserConfigFilePath(), vec);
        for (size_t i = 0; i < vector_total(vec); i++)
        {
            USERDATAINFO_PTR one = vector_get(vec, i);
            free(one->Name);
            free(one->PasswordHash);
            free(one->Type);
            free(one);
        }
        vector_free(vec);
        free(vec);
    }
    else
    {
        //用户按了取消按钮
        EnableWindow(context, true);
    }
}
typedef struct yinyue200_MainWindow_calctempdata
{
    LPWSTR MemberData;
    int Count;
} YINYUE200_MAINWINDOW_CALCTEMPDATA;
//统计函数
void calcmaindata(void* (*getmember)(void*), HWND hwnd)
{
    YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
    VECTOR_INIT(calc_temp_list);
    for (int i = 0; i < VECTOR_TOTAL(windata->UnsortedNowList); i++)
    {
        PRODUCTRECORD_PTR bbb = VECTOR_GET(windata->UnsortedNowList, PRODUCTRECORD_PTR, i);
        YINYUE200_MAINWINDOW_CALCTEMPDATA* aaafinal = NULL;
        for (int j = 0; j < VECTOR_TOTAL(calc_temp_list); j++)
        {
            YINYUE200_MAINWINDOW_CALCTEMPDATA* aaa = VECTOR_GET(calc_temp_list, YINYUE200_MAINWINDOW_CALCTEMPDATA*, j);
            wchar_t* mem = (wchar_t*)(getmember(bbb));
            if (aaa->MemberData == mem ||wcscmp(aaa->MemberData, mem) == 0)
            {
                aaafinal = aaa;
                aaa->Count++;
                break;
            }
        }
        if (aaafinal == NULL)
        {
            aaafinal = yinyue200_safemalloc(sizeof(YINYUE200_MAINWINDOW_CALCTEMPDATA));
            aaafinal->MemberData = getmember(bbb);
            aaafinal->Count = 1;
            vector_add(&calc_temp_list, aaafinal);
        }
    }
    YINYUE200_MAINWINDOW_CALCTEMPDATA* max = NULL;
    for (int k = 0; k < VECTOR_TOTAL(calc_temp_list); k++)
    {
        YINYUE200_MAINWINDOW_CALCTEMPDATA* aaa = VECTOR_GET(calc_temp_list, struct agent*, k);
        if (max == NULL || aaa->Count > max->Count)
        {
            max = aaa;
        }
    }
    if (max == NULL)
    {
        MessageBox(hwnd, L"没有统计结果", L"统计结果", 0);
    }
    else
    {
        MessageBox(hwnd, max->MemberData, L"统计结果", 0);
    }
}
typedef struct yinyue200_LoginCheckContext
{
    WPARAM wParam;
    HWND hwnd;
    vector* vec;
} YINYUE200_LOGINCHECKCONTEXT;
void logincheckmsg(void* context)
{
    if (yinyue200_LoganUserInfo!=NULL&&wcscmp(yinyue200_LoganUserInfo->Type, L"ADMIN")==0)
    {
        YINYUE200_LOGINCHECKCONTEXT* ycontext = context;
        HWND hwnd = ycontext->hwnd;
        WPARAM wParam = ycontext->wParam;
        switch (LOWORD(wParam))
        {
        case ID_MENU_ADDUSER:
        {
            EnableWindow(hwnd, false);
            CreateInputBoxWindow(L"输入要添加的用户名", addusernameinputed, hwnd, false);
            break;
        }
        case ID_MENU_CHANGEPWD:
        {
            EnableWindow(hwnd, false);
            CreateInputBoxWindow(L"输入要修改密码的用户名", addusernameinputed, hwnd, false);
            break;
        }
        case ID_MENU_REMOVEUSER:
        {
            EnableWindow(hwnd, false);
            CreateInputBoxWindow(L"输入要删除的用户", removeusernameinputed, hwnd, false);
            break;
        }
        case ID_MENU_IMPORT:
        {
            OPENFILENAME ofn = { 0 };
            wchar_t strFile[MAX_PATH];
            memset(strFile, 0, sizeof(char) * MAX_PATH);
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.lpstrFile = strFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_FILEMUSTEXIST;
            if (GetOpenFileName(&ofn))   //strFile得到用户所选择文件的路径和文件名 ; 
            {
                vector* vec = ProductRecordLoadToVector(strFile);
                for (size_t i = 0; i < vector_total(vec); i++)
                {
                    PRODUCTRECORD_PTR one = vector_get(vec, i);
                    vector_add(&yinyue200_ProductList, one);
                }
                vector_free(vec);
                free(vec);
            }
            break;
        }
        case ID_MENU_ADDRECORD:
        {
            CreateEditItemWindow(NULL,true);
            break;
        }
        default:
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                switch (LOWORD(wParam))
                {
                case ID_BUTTON_REMOVESELECTEDITEMS:
                {
                    YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
                    for (size_t i = 0; i < vector_total(ycontext->vec); i++)
                    {
                        PRODUCTRECORD_PTR productrecord = vector_get(ycontext->vec, i);
                        for (size_t i = 0; i < VECTOR_TOTAL(yinyue200_ProductList); i++)
                        {
                            PRODUCTRECORD_PTR allproduct = VECTOR_GET(yinyue200_ProductList, PRODUCTRECORD_PTR, i);
                            if (allproduct == productrecord)
                            {
                                VECTOR_DELETE(yinyue200_ProductList, i);
                                break;
                            }
                        }
                        for (size_t i = 0; i < VECTOR_TOTAL(windata->UnsortedNowList); i++)
                        {
                            PRODUCTRECORD_PTR allproduct = VECTOR_GET(windata->UnsortedNowList, PRODUCTRECORD_PTR, i);
                            if (allproduct == productrecord)
                            {
                                VECTOR_DELETE(windata->UnsortedNowList, i);
                                break;
                            }
                        }
                    }
                    UpdateCheckBoxInfo(hwnd, windata);
                    for (size_t i = 0; i < vector_total(ycontext->vec); i++)
                    {
                        PRODUCTRECORD_PTR productrecord = vector_get(ycontext->vec, i);
                        free(productrecord->Name);
                        free(productrecord->ProvideBy);
                        free(productrecord->RecievedBy);
                        free(productrecord->ResentBy);
                        free(productrecord->Signer);
                        free(productrecord->Type);
                        free(productrecord->State);
                        free(productrecord);
                    }
                    vector_free(ycontext->vec);
                    free(ycontext->vec);
                    break;
                }    

                default:
                    break;
                }
            }
                break;
            }
            break;
        }
    }
    else
    {
        MessageBox(NULL, L"需要管理员权限", NULL, 0);
    }
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {

        AddWindowCount();

        HMENU hMenubar = CreateMenu();
        HMENU hFile = CreateMenu();
        HMENU hFind = CreateMenu();
        HMENU hAdd = CreateMenu();
        HMENU hAddItem = CreateMenu();
        HMENU hAns = CreateMenu();
        HMENU hUsr = CreateMenu();
        HMENU hHelp = CreateMenu();

        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, L"文件");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFind, L"查询");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAdd, L"新增");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hAns, L"统计");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hUsr, L"用户管理");
        AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hHelp, L"帮助");

        AppendMenu(hHelp, MF_STRING, ID_MENU_VWS, L"GitHub");
        AppendMenu(hHelp, MF_STRING, ID_MENU_ABOUT, L"关于");
        AppendMenu(hAdd, MF_STRING, ID_MENU_ADDRECORD, L"添加单个条目");
        AppendMenu(hFile, MF_STRING, ID_MENU_SAVE, L"保存");
        AppendMenu(hFile, MF_STRING, ID_MENU_IMPORT, L"导入数据");
        AppendMenu(hFind, MF_STRING, ID_MENU_LOADALL, L"查询所有");
        AppendMenu(hFind, MF_STRING, ID_MENU_FLITER, L"筛选现有数据");
        AppendMenu(hFind, MF_STRING, ID_MENU_FLITERLOADALL, L"条件查询");
        AppendMenu(hUsr, MF_STRING, ID_MENU_REMOVEUSER, L"删除用户");
        AppendMenu(hUsr, MF_STRING, ID_MENU_ADDUSER, L"添加用户");
        AppendMenu(hUsr, MF_STRING, ID_MENU_CHANGEPWD, L"重设用户密码和权限");
        AppendMenu(hUsr, MF_STRING, ID_MENU_SHOWUSERSLIST, L"显示用户名单");
        AppendMenu(hAns, MF_STRING, ID_MENU_CALCMAINPROVIDER, L"显示主要供货商");
        AppendMenu(hAns, MF_STRING, ID_MENU_CALCMAINRECIVER, L"显示主要收货商");
        AppendMenu(hAns, MF_STRING, ID_MENU_CALCMAINRESENTER, L"显示主要退货商");
        AppendMenu(hAns, MF_STRING, ID_MENU_CALCMAINSIGNER, L"显示主要经手人");
        AppendMenu(hAns, MF_STRING, ID_MENU_CALCALLCASH, L"显示总利润");
        AppendMenu(hAns, MF_STRING, ID_MENU_CALCALLCOST, L"显示总成本");



        SetMenu(hwnd, hMenubar);

        HWND listview = Yinyue200_Main_CreateListView(hwnd);
        Yinyue200_Main_InitListView(hwnd,listview);

        int buttony = 500;
        int buttonx = 10;
        HWND hwndPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"按页显示",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_CHECKBOX_PAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx+=100;
        HWND hwndPrevPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"上一页",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_PREVPAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx += 100;
        HWND hwndPageInputEdit = CreateWindow(
            L"EDIT",  // Predefined class; Unicode assumed 
            L"1",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD| WS_BORDER | ES_LEFT,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_EDIT_PAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx += 100;
        HWND hwndNextPageButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"下一页",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_NEXTPAGE,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.
        buttonx += 100;
        HWND hwndRemoveSelectedItemsButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"删除所选",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
            buttonx,         // x position 
            buttony,         // y position 
            100,        // Button width
            50,        // Button height
            hwnd,     // Parent window
            ID_BUTTON_REMOVESELECTEDITEMS,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        YINYUE200_MAINWINDOWDATA* windata = malloc(sizeof(YINYUE200_MAINWINDOWDATA));
        if (windata)
        {
            memset(windata, 0, sizeof(YINYUE200_MAINWINDOWDATA));
            windata->sortcomindex = -1;
            windata->WindowHwnd = hwnd;
            SetProp(hwnd, YINYUE200_WINDOW_DATA, windata);
            UpdateCheckBoxInfo(hwnd, windata);
        }
        else
        {
            UnrecoveryableFailed();
        }
    }
    return 0;
    case WM_NOTIFY:
    {
        if (LOWORD(wParam) == ID_LISTVIEW_MAIN)
        {
            ListViewNotify(hwnd, lParam);
        }
        return 0;
    }
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_MENU_CALCMAINPROVIDER:
        {
            calcmaindata(yinyue200_GetProductRecordProvideBy, hwnd);
            break;
        }
        case ID_MENU_CALCMAINRECIVER:
        {
            calcmaindata(yinyue200_GetProductRecordRecievedBy, hwnd);
            break;
        }
        case ID_MENU_CALCMAINRESENTER:
        {
            calcmaindata(yinyue200_GetProductRecordResentBy, hwnd);
            break;
        }
        case ID_MENU_CALCMAINSIGNER:
        {
            calcmaindata(yinyue200_GetProductRecordSigner, hwnd);
            break;
        }
        case ID_MENU_CALCALLCASH:
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            double cash = 0.0;
            for (size_t i = 0; i < vector_total(&windata->UnsortedNowList); i++)
            {
                PRODUCTRECORD_PTR one = vector_get(&windata->UnsortedNowList, i);
                cash += (one->Price - one->Cost) * one->Count;
            }
            wchar_t strmsg[60];
            swprintf_s(strmsg, 60, L"售出利润：%lf", cash);
            MessageBox(hwnd, strmsg, L"统计结果", 0);
            break;
        }
        case ID_MENU_CALCALLCOST:
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            double cash = 0.0;
            for (size_t i = 0; i < vector_total(&windata->UnsortedNowList); i++)
            {
                PRODUCTRECORD_PTR one = vector_get(&windata->UnsortedNowList, i);
                cash += one->Cost * one->Count;
            }
            wchar_t strmsg[60];
            swprintf_s(strmsg, 60, L"总成本：%lf", cash);
            MessageBox(hwnd, strmsg, L"统计结果", 0);
            break;
        }
        case ID_MENU_VWS:
            ShellExecute(NULL, L"Open", L"https://github.com/yinyue200/SimpleStoreErp", NULL, NULL, SW_SHOWNORMAL);
            break;
        case ID_MENU_ABOUT:
            MessageBox(hwnd, L"版本 0.0.1\r\nA GUI program\r\nold fashion design\n\nCopyright (C) 2021 殷越\n"
                L"This program comes with ABSOLUTELY NO WARRANTY.\n"
                L"This is free software, and you are welcome to redistribute it under certain conditions.\n"
                L"See https://github.com/yinyue200/SimpleStoreErp for more details.", L"关于", 0);
            break;
        case ID_MENU_SAVE:
            yinyue200_ProductRecordSaveToFile(yinyue200_GetConfigFilePath(), &yinyue200_ProductList);
            MessageBox(hwnd, L"保存成功", L"消息", 0);
            break;
        case ID_MENU_LOADALL:
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                yinyue200_main_loadnowlist(hwnd, windata);
            }
            break;
        }
        case ID_MENU_FLITERLOADALL:
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                yinyue200_main_loadnowlist(hwnd, windata);
                CreateLoadDataFilterWindow(windata);
            }
            break;
        }
        case ID_MENU_FLITER:
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                CreateLoadDataFilterWindow(windata);
            }
            break;
        }
        case ID_MENU_IMPORT:
        case ID_MENU_ADDRECORD:
        case ID_MENU_REMOVEUSER:
        case ID_MENU_CHANGEPWD:
        case ID_MENU_ADDUSER:
        {
            YINYUE200_LOGINCHECKCONTEXT* lcc = yinyue200_safemalloc(sizeof(YINYUE200_LOGINCHECKCONTEXT));
            lcc->hwnd = hwnd;
            lcc->wParam = wParam;
            CreateLoginWindow(GetNowLoginedUserName(), logincheckmsg, lcc);
            break;
        }
        case ID_MENU_SHOWUSERSLIST:
        {
            wchar_t* buf = yinyue200_safemalloc(2000);

            wchar_t *now = buf;

            vector* vec = UserRecordLoadToVector(yinyue200_GetUserConfigFilePath());
            for (size_t i = 0; i < vector_total(vec); i++)
            {
                USERDATAINFO_PTR one = vector_get(vec, i);
                int ret = swprintf_s(now, 1000 - (now - buf), L"%s %s\n", one->Name, one->Type);
                now += ret;
                free(one->Name);
                free(one->PasswordHash);
                free(one->Type);
                free(one);
            }
            vector_free(vec);
            free(vec);

            MessageBox(NULL, buf, L"用户列表", 0);

            free(buf);
            break;
        }
        default:
        {
            YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
            if (windata)
            {
                switch (HIWORD(wParam))
                {
                case BN_CLICKED:
                {
                    switch (LOWORD(wParam))
                    {
                    case ID_CHECKBOX_PAGE:
                        UpdateCheckBoxInfo(hwnd, windata);
                        break;
                    case ID_BUTTON_NEXTPAGE:
                        windata->pagestart += MAIN_DISPLAYPAGESIZE;
                        UpdateCheckBoxInfo(hwnd, windata);
                        break;
                    case ID_BUTTON_PREVPAGE:
                        windata->pagestart -= MAIN_DISPLAYPAGESIZE;
                        if (windata->pagestart < 0)
                            windata->pagestart = 0;
                        UpdateCheckBoxInfo(hwnd, windata);
                        break;
                    case ID_BUTTON_REMOVESELECTEDITEMS:
                    {
                        HWND hListView = GetDlgItem(hwnd, ID_LISTVIEW_MAIN);
                        int iPos = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);
                        if (iPos == -1)
                        {
                            MessageBox(hwnd, L"当前没有选择任何项", NULL, 0);
                        }
                        else
                        {
                            vector* vec = yinyue200_safemalloc(sizeof(vector));
                            vector_init(vec);

                            windata->sortstate = 0;
                            while (iPos != -1) {
                                // iPos is the index of a selected item
                                // do whatever you want with it
                                PRODUCTRECORD_PTR productrecord = VECTOR_GET(windata->PagedNowList, PRODUCTRECORD_PTR, iPos);
                                vector_add(vec, productrecord);

                                // Get the next selected item
                                iPos = ListView_GetNextItem(hListView, iPos, LVNI_SELECTED);
                            }
                            wchar_t warningmsg[50];
                            swprintf(warningmsg, 50, L"你确定要删除 %d 条记录吗？", vector_total(vec));
                            if (MessageBox(hwnd, warningmsg, L"提示", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
                            {
                                YINYUE200_LOGINCHECKCONTEXT* lcc = yinyue200_safemalloc(sizeof(YINYUE200_LOGINCHECKCONTEXT));
                                lcc->hwnd = hwnd;
                                lcc->wParam = wParam;
                                lcc->vec = vec;
                                CreateLoginWindow(GetNowLoginedUserName(), logincheckmsg, lcc);
                            }
                            else
                            {
                                vector_free(vec);
                                free(vec);
                            }
                        }    

                    }
                    default:
                        break;
                    }
                }
                break;
                case EN_CHANGE:
                {
                    switch (LOWORD(wParam))
                    {
                    case ID_EDIT_PAGE:
                    {
                        HWND editpagectrl = GetDlgItem(hwnd, ID_EDIT_PAGE);
                        LPWSTR pagestr = CreateWstrForWindowText(editpagectrl);
                        int pagenum;
                        int startpos;
                        if (swscanf(pagestr, L"%d", &pagenum) == 1 &&
                            ((startpos = (pagenum - 1) * MAIN_DISPLAYPAGESIZE) < VECTOR_TOTAL(windata->NowList) || startpos == 0)
                            && startpos >= 0
                            )//验证输入的页码是否有效
                        {
                            if (windata->pagestart != startpos)//只有当实际下标起点改变时才执行操作
                            {
                                windata->pagestart = startpos;
                                UpdateCheckBoxInfo(hwnd, windata);
                            }
                            PostMessage(GetDlgItem(hwnd, ID_STATUSBAR_MAIN), SB_SETTEXT, MAKEWPARAM(1, 0), L"");
                        }
                        else
                        {
                            PostMessage(GetDlgItem(hwnd, ID_STATUSBAR_MAIN), SB_SETTEXT, MAKEWPARAM(1, 0), L"无效的页码");
                        }
                    }
                    break;
                    default:
                        break;
                    }
                }
                break;
                }
            }
        }
            break;
        }
    }
    return 0;
    case WM_DESTROY:
    {
        YINYUE200_MAINWINDOWDATA* windata = GetProp(hwnd, YINYUE200_WINDOW_DATA);
        if (windata)
        {
            free(windata);
        }
        RemoveProp(hwnd, YINYUE200_WINDOW_DATA);
        DecreaseWindowCount();
        PostQuitMessage(0);//当主函数退出时发送退出程序消息
        return 0;
    }
    case WM_SIZE:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        int buttony = y - 75;
        OnStatusbarSize(hwnd, wParam, x, y);
        int buttonx = 10;
        SetWindowPos(GetDlgItem(hwnd, ID_LISTVIEW_MAIN), HWND_TOP, -1, -1, x, buttony, SWP_NOMOVE);
        SetWindowPos(GetDlgItem(hwnd, ID_CHECKBOX_PAGE), HWND_TOP, buttonx, buttony, -1, -1, SWP_NOSIZE);
        buttonx += 100;
        SetWindowPos(GetDlgItem(hwnd, ID_BUTTON_PREVPAGE), HWND_TOP, buttonx, buttony, -1, -1, SWP_NOSIZE);
        buttonx += 100;
        SetWindowPos(GetDlgItem(hwnd, ID_EDIT_PAGE), HWND_TOP, buttonx, buttony, -1, -1, SWP_NOSIZE);
        buttonx += 100;
        SetWindowPos(GetDlgItem(hwnd, ID_BUTTON_NEXTPAGE), HWND_TOP, buttonx, buttony, -1, -1, SWP_NOSIZE);
        buttonx += 100;
        SetWindowPos(GetDlgItem(hwnd, ID_BUTTON_REMOVESELECTEDITEMS), HWND_TOP, buttonx, buttony, -1, -1, SWP_NOSIZE);
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);



        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
