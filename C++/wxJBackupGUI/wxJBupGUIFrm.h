///-----------------------------------------------------------------
///
/// @file      wxJBupGUIFrm.h
/// @author    Josh Bosley
/// Created:   4/1/2014 6:29:28 PM
/// @section   DESCRIPTION
///            Jbup class declaration
///
///------------------------------------------------------------------

#ifndef __JBUP_H__
#define __JBUP_H__

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/frame.h>
#else
	#include <wx/wxprec.h>
#endif

//Do not add custom headers between 
//Header Include Start and Header Include End.
//wxDev-C++ designer will remove them. Add custom headers after the block.
////Header Include Start
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/combobox.h>
////Header Include End

//// Extra Include Begin
#include <time.h>
#include <wx/textfile.h>

// For Path handles
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif
//// Extra Include End


////Dialog Style Start
#undef Jbup_STYLE
#define Jbup_STYLE wxSIMPLE_BORDER | wxTRANSPARENT_WINDOW | wxCAPTION | wxSYSTEM_MENU | wxMINIMIZE_BOX | wxCLOSE_BOX
////Dialog Style End

// Global information
#define DEFAULT_BOX_LOCATION   "C:\\Program Files\\JBackupUtility\\box\\"

#define JOB_LIST "JConfig.txt"

#define MAX_LINES   3
#define MAX_FPATH 255
#define MAX_JFILE  50
#define MAX_TJOBS   3
#define  MAX_JOBS  25
#define  MAX_NAME  32
#define  MAX_DATE  20

#define BUGS        1




class Jbup : public wxFrame
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		Jbup(wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("wxJBupGUI"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = Jbup_STYLE);
		virtual ~Jbup();
		void EditClick(wxCommandEvent& event);
		
	private:
		//Do not add custom control declarations between
		//GUI Control Declaration Start and GUI Control Declaration End.
		//wxDev-C++ will remove them. Add custom code after the block.
		////GUI Control Declaration Start
		wxDirDialog *SourceSelection;
		wxFileDialog *WxSaveFileDialog1;
		wxDirDialog *DestinationSelection;
		wxMessageDialog *WxMessageDialog1;
		wxStaticText *IntervalLabel;
		wxComboBox *IntervalDropdown;
		wxStaticText *DayText;
		wxStaticText *MonthTextLabel;
		wxComboBox *MonthDaysDropdown;
		wxComboBox *MonthsDropdown;
		wxStaticText *TimeDisplayText;
		wxCheckBox *CheckBoxDays;
		wxCheckBox *CheckBoxMonths;
		wxButton *EditTitle;
		wxStaticText *ErrorText;
		wxButton *SaveButton;
		wxButton *EditDestination;
		wxButton *EditSource;
		wxTextCtrl *DestinationPathBox;
		wxTextCtrl *SourcePathBox;
		wxStaticText *Destination;
		wxStaticText *Source;
		wxStaticText *TitleLabel;
		wxTextCtrl *TitleBox;
		wxButton *Load;
		wxComboBox *BackupSelect;
		////GUI Control Declaration End
		
	private:
		//Note: if you receive any error with these enum IDs, then you need to
		//change your old form code that are based on the #define control IDs.
		//#defines may replace a numeric value for the enum names.
		//Try copy and pasting the below block in your old form header files.
		enum
		{
			////GUI Enum Control ID Start
			ID_INTERVALLABEL = 1039,
			ID_INTERVALDROPDOWN = 1038,
			ID_DAYTEXT = 1037,
			ID_MONTHTEXTLABEL = 1036,
			ID_MONTHDAYSDROPDOWN = 1035,
			ID_MONTHSDROPDOWN = 1034,
			ID_TIMEDISPLAYTEXT = 1032,
			ID_CHECKBOXDAYS = 1029,
			ID_CHECKBOXMONTHS = 1028,
			ID_EDITTITLE = 1027,
			ID_ERRORTEXT = 1026,
			ID_SAVEBUTTON = 1024,
			ID_EDITDESTINATION = 1021,
			ID_EDITSOURCE = 1019,
			ID_DESTINATIONPATHBOX = 1018,
			ID_SOURCEPATHBOX = 1017,
			ID_DESTINATION = 1012,
			ID_SOURCE = 1011,
			ID_TITLELABEL = 1005,
			ID_TITLEBOX = 1004,
			ID_LOAD = 1002,
			ID_BACKUPSELECT = 1001,
			////GUI Enum Control ID End
			ID_DUMMY_VALUE_ //don't remove this value unless you have other enum values
		};
		
	private:
        
		void OnClose(wxCloseEvent& event) ;
		void CreateGUIControls()          ;
		void LoadJobInformation()         ;

        // Job structure and list (jlist) for handling job info
        struct jobs
        {
	        char title            [MAX_NAME];
        	char from            [MAX_FPATH];
        	char to              [MAX_FPATH];
        	char dateTime         [MAX_DATE];
	        char jobfile         [MAX_JFILE];
        };

        typedef struct jobs jobs;
        
        int number_of_jobs;
        int current_job;
        
        jobs jlist[MAX_JOBS];
        
        // Temp state edited by user before saving. 
        struct TempJob
        {
	        wxString title         ;
	        wxString source        ;
	        wxString dest          ;
	        wxString dateTime      ;
	        wxString jobfile       ;
	        int      being_used    ;
        };

        typedef struct TempJob TempList;
                
        int current_temp_job;

        // TempJob TempJobs[MAX_TJOBS];
        TempList TempJobs[MAX_TJOBS];
        
        // Current Directory Information
        char CurrentWorkingPath[MAX_FPATH];
        
        // Events
		void EditSourceClick(wxCommandEvent& event);
		void EditDestinationClick(wxCommandEvent& event);
		void TitleBoxEnter(wxCommandEvent& event);
		void SaveButtonClick(wxCommandEvent& event);
		void TitleBoxUpdated(wxCommandEvent& event);
		void EditTitleClick(wxCommandEvent& event);
		void TitleBoxUpdated0(wxCommandEvent& event);
		void CheckBoxMonthsClick(wxCommandEvent& event);
		void CheckBoxDaysClick(wxCommandEvent& event);




};

#endif
