///-----------------------------------------------------------------
///
/// @file      wxJBupGUIFrm.cpp
/// @author    Josh Bosley
/// Created:   4/1/2014 6:29:28 PM
/// @section   DESCRIPTION
///            Jbup class implementation
///
///------------------------------------------------------------------

#include "wxJBupGUIFrm.h"


//Do not add custom headers between
//Header Include Start and Header Include End
//wxDev-C++ designer will remove them
////Header Include Start
////Header Include End


//----------------------------------------------------------------------------
// Jbup
//----------------------------------------------------------------------------
//Add Custom Events only in the appropriate block.
//Code added in other places will be removed by wxDev-C++
////Event Table Start
BEGIN_EVENT_TABLE(Jbup,wxFrame)
	////Manual Code Start
	////Manual Code End
	
	EVT_CLOSE(Jbup::OnClose)
	EVT_CHECKBOX(ID_CHECKBOXDAYS,Jbup::CheckBoxDaysClick)
	EVT_CHECKBOX(ID_CHECKBOXMONTHS,Jbup::CheckBoxMonthsClick)
	EVT_BUTTON(ID_EDITTITLE,Jbup::EditTitleClick)
	EVT_BUTTON(ID_SAVEBUTTON,Jbup::SaveButtonClick)
	EVT_BUTTON(ID_EDITDESTINATION,Jbup::EditDestinationClick)
	EVT_BUTTON(ID_EDITSOURCE,Jbup::EditSourceClick)
	
	EVT_TEXT(ID_TITLEBOX,Jbup::TitleBoxUpdated0)
	EVT_BUTTON(ID_LOAD,Jbup::EditClick)
END_EVENT_TABLE()
////Event Table End

// Error string
wxString ERROR_OUT;


Jbup::Jbup(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style)
: wxFrame(parent, id, title, position, size, style)
{
	CreateGUIControls();
	
	// Load job info
	LoadJobInformation();
	
}

Jbup::~Jbup()
{
}

void Jbup::CreateGUIControls()
{
	//Do not add custom code between
	//GUI Items Creation Start and GUI Items Creation End
	//wxDev-C++ designer will remove them.
	//Add the custom code before or after the blocks
	////GUI Items Creation Start

	SourceSelection =  new wxDirDialog(this, _("Choose a directory"), _(""));

	WxSaveFileDialog1 =  new wxFileDialog(this, _("Choose a file"), _(""), _(""), _("*.*"), wxFD_SAVE);

	DestinationSelection =  new wxDirDialog(this, _("Choose a directory"), _(""));

	WxMessageDialog1 =  new wxMessageDialog(this, _(""), _("Message box"));

	IntervalLabel = new wxStaticText(this, ID_INTERVALLABEL, _("Interval :"), wxPoint(168, 208), wxDefaultSize, 0, _("IntervalLabel"));
	IntervalLabel->Show(false);
	IntervalLabel->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	wxArrayString arrayStringFor_IntervalDropdown;
	IntervalDropdown = new wxComboBox(this, ID_INTERVALDROPDOWN, _("..."), wxPoint(224, 200), wxSize(49, 27), arrayStringFor_IntervalDropdown, 0, wxDefaultValidator, _("IntervalDropdown"));
	IntervalDropdown->Show(false);
	IntervalDropdown->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	DayText = new wxStaticText(this, ID_DAYTEXT, _("Start Day :"), wxPoint(24, 240), wxDefaultSize, 0, _("DayText"));
	DayText->Show(false);
	DayText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	MonthTextLabel = new wxStaticText(this, ID_MONTHTEXTLABEL, _("Start Month :"), wxPoint(8, 208), wxDefaultSize, 0, _("MonthTextLabel"));
	MonthTextLabel->Show(false);
	MonthTextLabel->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	wxArrayString arrayStringFor_MonthDaysDropdown;
	MonthDaysDropdown = new wxComboBox(this, ID_MONTHDAYSDROPDOWN, _("..."), wxPoint(96, 232), wxSize(49, 27), arrayStringFor_MonthDaysDropdown, wxCB_DROPDOWN, wxDefaultValidator, _("MonthDaysDropdown"));
	MonthDaysDropdown->Show(false);
	MonthDaysDropdown->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	wxArrayString arrayStringFor_MonthsDropdown;
	MonthsDropdown = new wxComboBox(this, ID_MONTHSDROPDOWN, _("..."), wxPoint(96, 200), wxSize(49, 27), arrayStringFor_MonthsDropdown, wxCB_DROPDOWN, wxDefaultValidator, _("MonthsDropdown"));
	MonthsDropdown->Show(false);
	MonthsDropdown->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	TimeDisplayText = new wxStaticText(this, ID_TIMEDISPLAYTEXT, _("Shedule :"), wxPoint(8, 176), wxDefaultSize, 0, _("TimeDisplayText"));
	TimeDisplayText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	CheckBoxDays = new wxCheckBox(this, ID_CHECKBOXDAYS, _("Day(s)"), wxPoint(224, 176), wxSize(97, 17), 0, wxDefaultValidator, _("CheckBoxDays"));
	CheckBoxDays->Enable(false);
	CheckBoxDays->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	CheckBoxMonths = new wxCheckBox(this, ID_CHECKBOXMONTHS, _("Monthly"), wxPoint(96, 176), wxSize(97, 17), 0, wxDefaultValidator, _("CheckBoxMonths"));
	CheckBoxMonths->Enable(false);
	CheckBoxMonths->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	EditTitle = new wxButton(this, ID_EDITTITLE, _("Edit"), wxPoint(320, 32), wxSize(75, 25), 0, wxDefaultValidator, _("EditTitle"));
	EditTitle->Enable(false);
	EditTitle->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	ErrorText = new wxStaticText(this, ID_ERRORTEXT, _("ErrorText"), wxPoint(504, 32), wxDefaultSize, 0, _("ErrorText"));
	ErrorText->Show(false);
	ErrorText->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	SaveButton = new wxButton(this, ID_SAVEBUTTON, _("Save"), wxPoint(88, 304), wxSize(67, 25), 0, wxDefaultValidator, _("SaveButton"));
	SaveButton->Enable(false);
	SaveButton->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	EditDestination = new wxButton(this, ID_EDITDESTINATION, _("Edit"), wxPoint(319, 144), wxSize(76, 25), 0, wxDefaultValidator, _("EditDestination"));
	EditDestination->Enable(false);
	EditDestination->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	EditSource = new wxButton(this, ID_EDITSOURCE, _("Edit"), wxPoint(318, 85), wxSize(75, 25), 0, wxDefaultValidator, _("EditSource"));
	EditSource->Enable(false);
	EditSource->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	DestinationPathBox = new wxTextCtrl(this, ID_DESTINATIONPATHBOX, _("..."), wxPoint(8, 144), wxSize(304, 22), 0, wxDefaultValidator, _("DestinationPathBox"));
	DestinationPathBox->Enable(false);
	DestinationPathBox->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	SourcePathBox = new wxTextCtrl(this, ID_SOURCEPATHBOX, _("..."), wxPoint(8, 88), wxSize(304, 22), 0, wxDefaultValidator, _("SourcePathBox"));
	SourcePathBox->Enable(false);
	SourcePathBox->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	Destination = new wxStaticText(this, ID_DESTINATION, _("Destination : "), wxPoint(8, 120), wxDefaultSize, 0, _("Destination"));
	Destination->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	Source = new wxStaticText(this, ID_SOURCE, _("Source : "), wxPoint(8, 60), wxDefaultSize, 0, _("Source"));
	Source->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	TitleLabel = new wxStaticText(this, ID_TITLELABEL, _("Title : "), wxPoint(9, 9), wxDefaultSize, 0, _("TitleLabel"));
	TitleLabel->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	TitleBox = new wxTextCtrl(this, ID_TITLEBOX, _("..."), wxPoint(8, 32), wxSize(304, 22), 0, wxDefaultValidator, _("TitleBox"));
	TitleBox->Enable(false);
	TitleBox->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	Load = new wxButton(this, ID_LOAD, _("Load"), wxPoint(8, 304), wxSize(75, 25), 0, wxDefaultValidator, _("Load"));
	Load->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	wxArrayString arrayStringFor_BackupSelect;
	BackupSelect = new wxComboBox(this, ID_BACKUPSELECT, _("Select Backup"), wxPoint(9, 269), wxSize(302, 27), arrayStringFor_BackupSelect, 0, wxDefaultValidator, _("BackupSelect"));
	BackupSelect->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxNORMAL, false, _("Times New Roman")));

	SetTitle(_("wxJBupGUI"));
	SetIcon(wxNullIcon);
	SetSize(8,8,452,403);
	Center();
	
	////GUI Items Creation End
	
	// Setup Debugging
	if(BUGS)   ErrorText->Show();
	
	/*
    
            NOT CURRENTLY IN USE, BUT KEEP FOR NOW \/
    */
	
	// Set Working Directory
	if (!GetCurrentDir(CurrentWorkingPath, sizeof(CurrentWorkingPath)))
        strcpy(CurrentWorkingPath, "C:\\");
        
    CurrentWorkingPath[sizeof(CurrentWorkingPath) - 1] = '\0'; 
}

void Jbup::OnClose(wxCloseEvent& event)
{
	Destroy();
}

/*
 * EditClick labeled as LOAD
 * Selects the job in jlist;
 */
void Jbup::EditClick(wxCommandEvent& event)
{
    current_job      = 0 ;
    current_temp_job = 0 ;
    wxString temp ;
         
    // Set temp job to hold what user is changing.
    TempJobs[ current_temp_job].being_used = 1;
    
    for(current_job; current_job < number_of_jobs; current_job++)
    {
        temp = jlist[ current_job ].title;
        if( temp == BackupSelect->GetValue() )
        {
            // Set Data
            TempJobs[ current_temp_job].title       = jlist[ current_job ].title;
            TempJobs[ current_temp_job].source      = jlist[ current_job ].from;
            TempJobs[ current_temp_job].dest        = jlist[ current_job ].to;
            TempJobs[ current_temp_job].dateTime    = jlist[ current_job ].dateTime;
            TempJobs[ current_temp_job].jobfile     = jlist[ current_job ].jobfile;
            
            // Set User Iterface
            TitleBox->SetValue( BackupSelect->GetValue() );
            SourcePathBox->SetValue( jlist[ current_job ].from );
            DestinationPathBox->SetValue( jlist[ current_job ].to   );
            
            // Enable editing
            EditDestination->Enable();
            EditSource->Enable();
            EditTitle->Enable();
            CheckBoxMonths->Enable();
            CheckBoxDays->Enable();
        }
    }    
}

/*                   
 * Load Combo Box
 */
void Jbup::LoadJobInformation()
{
     int x            = 0     ;
     int line_count   = 0     ;
     number_of_jobs   = 0     ;
     current_job      = 0     ;
     char job_file[MAX_NAME]  ;
     char    temp[MAX_FPATH]  ;
     wxString wxTemp;
     
     // Make sure dropdown is clear before appending.
     if( !BackupSelect->IsListEmpty() )
          BackupSelect->Clear();

    // Open the jconfig file
     FILE *jlist_fp = fopen( JOB_LIST , "r" ); 
     
     if (jlist_fp == NULL)
     {
            x = MAX_JOBS;
     }

     //   Go Through each job, and load. | x Ensures < MAX_JOBS and exits if error
     while ( fgets( job_file, sizeof(job_file), jlist_fp) && x < MAX_JOBS )
     {
           char *blarg1 = strstr(job_file, "\n");
           if (blarg1 != NULL) *blarg1 = 0;
           // Open the file pulled from JConfig
            FILE *job_fp = fopen( job_file , "r" );
            
            if (job_fp == NULL)
            {
                // Couldnt open JobFile, So flag for JConfig removal
                line_count = MAX_LINES;
                strcpy(jlist[ x ].jobfile, "_REMOVE_");
                         
            }
            else  // If it was opened, then go ahead and populate information
            {
                  // Store the job file for later in struct
                  strcpy(jlist[ x ].jobfile, job_file);              
            
                // line_count Handles the placement of data into the structure
                while ( fgets( temp, sizeof(temp), job_fp) && line_count <= MAX_LINES)
                {
                     char *control = strstr(temp, "\n");
                    if (control != NULL) *control = 0;
                    if ( line_count == 0 ){ strcpy( jlist[ x ].title,     temp ) ; }
                    if ( line_count == 1 ){ strcpy( jlist[ x ].from,      temp ) ; }
                    if ( line_count == 2 ){ strcpy( jlist[ x ].to,        temp ) ; }
                    if ( line_count == 3 ){ strcpy( jlist[ x ].dateTime,  temp ) ; }
                    line_count++;
                }
                fclose(job_fp);
                fflush(job_fp);
            
                // Set combobox options
                wxTemp = jlist[ x ].title;
                BackupSelect->Append(_( wxTemp ));
                // Update control
                line_count = 0;
                x++;
                
            }// < END ELSE >
     } // < END POPULATE JOBS WHILE >
     fclose(jlist_fp)       ;
     fflush(jlist_fp)       ;
     number_of_jobs  = x    ;
}

/*
* EditSourceClick
*/
void Jbup::EditSourceClick(wxCommandEvent& event)
{
    int i = 0;
    int result;
    wxString temp_path;
    
    // Enable Save Button
    SaveButton->Enable();
    
    // Setup Dialog, and display
    wxDirDialog SelectSourceDialog(SourceSelection, "Choose Source Directory", "",
                        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    SelectSourceDialog.SetPath( jlist[ current_job ].from );
    result = SelectSourceDialog.ShowModal();
    
    // Get user selection
    if ( result == wxID_OK )
    {
       TempJobs[ current_temp_job].source = SelectSourceDialog.GetPath();
    
    // Edit source to have double slashes.
        for( i = 0; i< strlen(TempJobs[ current_temp_job].source); i++)
        {
            temp_path.Append( TempJobs[ current_temp_job].source[ i ] );
        
            if ( TempJobs[ current_temp_job].source[ i ] == '\\')
            {
                temp_path.Append('\\');
            }
        }
        // Set the current temp job data to the new edited path and display
        TempJobs[ current_temp_job].source = temp_path;
    }
    SourcePathBox->SetValue( TempJobs[ current_temp_job].source );
}

/*
* EditDestinationClick
*/
void Jbup::EditDestinationClick(wxCommandEvent& event)
{
    int i = 0;
    int result;
    wxString temp_path;
    
    // Enable Save Button
    SaveButton->Enable();
    
    // Setup Dialog and display
    wxDirDialog SelectDestDialog(DestinationSelection, "Choose Destination Directory", "",
                        wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    SelectDestDialog.SetPath( jlist[ current_job ].to );
    result = SelectDestDialog.ShowModal();
    
    if ( result == wxID_OK )
    {
        // Get user selection
        TempJobs[ current_temp_job].dest = SelectDestDialog.GetPath();
    
        // Edit source to have double slashes.
        for( i; i< strlen(TempJobs[ current_temp_job].dest); i++)
        {
            temp_path.Append( TempJobs[ current_temp_job].dest[ i ] );
        
            if ( TempJobs[ current_temp_job].dest[ i ] == '\\')
            {
                temp_path.Append('\\');
            }
        }
        // Set the temp job info
        TempJobs[ current_temp_job].dest = temp_path;
    }
    
    DestinationPathBox->SetValue( TempJobs[ current_temp_job].dest );
}

/*
 * SaveButtonClick
 */
void Jbup::SaveButtonClick(wxCommandEvent& event)
{
    // ***  UPDATE IF DATETIME CHANGE ***
    
    int i = 0;
    wxString temp_path; 
    if( CheckBoxMonths->IsChecked() || CheckBoxDays->IsChecked() )
    {
            // Get Datetime info from job for editing
            int mt, d, y, h, m, s, update_index;
            
            time_t epoch_time;
            struct tm *time_p = NULL;
            epoch_time = time( NULL );
            time_p = localtime( &epoch_time );
            
            #define TIME_SEPARATOR
            sscanf(TempJobs[ current_temp_job].dateTime,
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR
            "%d"TIME_SEPARATOR"%d"TIME_SEPARATOR"%d",
            &y, &mt, &d, &h, &m, &s, &update_index);
            #undef TIME_SEPARATOR
        
            // Specific Cases
            if ( CheckBoxMonths->IsChecked() )
            {   
                y = (-1); h = (-1); m = (-1); s = (-1); 
                
                // Month Handle
                mt = wxAtoi(MonthsDropdown->GetValue());
                if ( mt == 0 )
                    mt = (time_p->tm_mon)+1 ;
                // Day Handle
	            d  = wxAtoi(MonthDaysDropdown->GetValue());
                if (  d == 0 )
                    d = time_p->tm_mday;
                // Set Update index
                update_index = 1;
            }
            else if ( CheckBoxDays->IsChecked() ) 
            {
                y = (-1); mt = (-1); h = (-1); m = (-1); s = (-1); 
                
                // Day Handle
                d = time_p->tm_mday;
                
                // Handle Updateindex
                update_index  = wxAtoi(IntervalDropdown->GetValue());
                if ( update_index == 0 )
                     update_index  = 1;
            }
            TempJobs[ current_temp_job ].dateTime =        wxString::Format(wxT("%i "), y )            ;
            TempJobs[ current_temp_job ].dateTime.Append(  wxString::Format(wxT("%i "), mt)           );
            TempJobs[ current_temp_job ].dateTime.Append(  wxString::Format(wxT("%i "), d )           );
            TempJobs[ current_temp_job ].dateTime.Append(  wxString::Format(wxT("%i "), h )           );
            TempJobs[ current_temp_job ].dateTime.Append(  wxString::Format(wxT("%i "), m )           );
            TempJobs[ current_temp_job ].dateTime.Append(  wxString::Format(wxT("%i "), s )           );
            TempJobs[ current_temp_job ].dateTime.Append(  wxString::Format(wxT("%i" ), update_index) );
    }
    // *** END UPDATE *** 
    
    // Get where user wants to save the file
    wxFileDialog 
        saveFileDialog(this, _("Save JBK file"), "", "",
                       "JBK files (*.jbk)|*.jbk", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
                                            
        saveFileDialog.SetPath( DEFAULT_BOX_LOCATION );

    if (saveFileDialog.ShowModal() == wxID_CANCEL)
        return;
        
    // Create JConfig acceptable filename
    TempJobs[ current_temp_job ].jobfile = "box/" + saveFileDialog.GetFilename();

    // Open and write Job file
    wxFile output_file( TempJobs[ current_temp_job].jobfile , wxFile::write);

    if( output_file.IsOpened() )
    {
    	    output_file.Write(TempJobs[ current_temp_job].title    );
            output_file.Write("\r\n");
	        output_file.Write(TempJobs[ current_temp_job].source   );
            output_file.Write("\r\n");
	        output_file.Write(TempJobs[ current_temp_job].dest     );
            output_file.Write("\r\n");
	        output_file.Write(TempJobs[ current_temp_job].dateTime );
            output_file.Write("\r\n");
            output_file.Write("\r\n");
            
            output_file.Close();
    }
    else
    {
        ERROR_OUT = " Problem with opening file \n";
        if(BUGS)   ErrorText->SetLabel(ERROR_OUT);
        return;
    }
    
    // Update Job List File ( JConfig )
     wxFile out_job_file( JOB_LIST , wxFile::write);
     if( out_job_file.IsOpened() )
     {
        int name_update_required = 1;
        for( i=0; i < number_of_jobs; i++)
        { 
            if ( strcmp( jlist[ i ].jobfile, "_REMOVE_") > 0 )
            {
                // Write to file
                out_job_file.Write(  jlist[ i ].jobfile   );
                out_job_file.Write(  "\n"                 );
                
                // Make sure it needs to be written. 
                int subi = 0  ;
                wxString moot ;
                for(subi; subi<number_of_jobs; subi++)
                {
                    moot = jlist[ subi ].jobfile;
                    if ( moot == TempJobs[ current_temp_job ].jobfile )
                        name_update_required = 0;
                }
                
                if ( name_update_required == 1 )
                {
                    // Write it
                    name_update_required = 0;
                    out_job_file.Write(  TempJobs[ current_temp_job ].jobfile );
                    out_job_file.Write(  "\n" );
                }
            }
            else
            {
                // Don't write to file
            }
        }   
        out_job_file.Close();
    }
    else
    {
        ERROR_OUT = " Problem with opening JConfig \n";
        if(BUGS)   ErrorText->SetLabel(ERROR_OUT);
        return; 
    }

     // Reload the combo box options
     LoadJobInformation();

     // Disable Buttons
     TitleBox->Disable();
     SaveButton->Disable();
     EditTitle->Disable();
     EditSource->Disable();
     EditDestination->Disable();
     
     // Reset and disable Monthly options
     CheckBoxMonths->Disable();
     CheckBoxMonths->SetValue(false);
	 MonthsDropdown->Hide();
	 MonthDaysDropdown->Hide();
	 MonthTextLabel->Hide();

     // Reset and disable Daily options
     CheckBoxDays->Disable();
	 CheckBoxDays->SetValue(false);
     IntervalDropdown->Hide();
	 IntervalLabel->Hide();
}

/*
 * EditTitleClick
 */
void Jbup::EditTitleClick(wxCommandEvent& event)
{
    TitleBox->Enable();
    SaveButton->Enable();
}

/*
 * TitleBoxUpdated0
 */
void Jbup::TitleBoxUpdated0(wxCommandEvent& event)
{
    
        int i, j;
    wxString ntmp;
    wxString temp = TitleBox->GetValue();
    
    for(i = 0, j = 1; i < number_of_jobs; i++)
    {
        ntmp = jlist[ i ].title; 
        if( temp == ntmp)
        {
            temp = temp + " " ;
            j++;
        }
    }
    TempJobs[ current_temp_job].title = temp;
    
//	TempJobs[ current_temp_job].title = TitleBox->GetValue();
}

/*
 * CheckBoxMonthsClick
 */
void Jbup::CheckBoxMonthsClick(wxCommandEvent& event)
{
    // Build Options
    int i = 0;
    
    for( i; i < 30; i++ )
    {
        MonthDaysDropdown->Append( wxString::Format(wxT("%d"), (int)(i+1)) ); 
    }
    
    for( i = 0; i < 12; i++ )
    {
        MonthsDropdown->Append( wxString::Format(wxT("%d"), (int)(i+1)) );    
    }

    // Show options
	MonthsDropdown->Show();
	MonthDaysDropdown->Show();
	MonthTextLabel->Show();
	DayText->Show();
	
	SaveButton->Enable();
	
	// Hide other options
	CheckBoxDays->SetValue(false);
	IntervalDropdown->Hide();
	IntervalLabel->Hide();
	
	// Disallow Unchecking
	CheckBoxMonths->Disable();
	CheckBoxDays->Enable();
}

/*
 * CheckBoxDaysClick
 */
void Jbup::CheckBoxDaysClick(wxCommandEvent& event)
{
    // Build Options
    int i = 0;
    
    for( i; i < 30; i++ )
    {
        IntervalDropdown->Append( wxString::Format(wxT("%d"), (int)(i+1)) );    
    }
    
    // Show options
    IntervalDropdown->Show();
	IntervalLabel->Show();
	
	SaveButton->Enable();

	// Hide other options
	CheckBoxMonths->SetValue(false);
	MonthsDropdown->Hide();
	MonthDaysDropdown->Hide();
	MonthTextLabel->Hide();
	DayText->Hide();
	
	// Disallow Unchecking
	CheckBoxDays->Disable();
	CheckBoxMonths->Enable();
}
