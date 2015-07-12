package ixi;

import java.text.*;
import java.util.Calendar;

import org.eclipse.swt.widgets.DirectoryDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.DateTime;
import org.eclipse.wb.swt.SWTResourceManager;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.eclipse.swt.widgets.Combo;

public class ixiMain extends ixiInitiator
{
	/*				Interface State Information					*/
    public static Calendar cal = Calendar.getInstance();
    public boolean CreateNewBackup;
    
    ixi.ixiEngine.Backup CurrentJob;
	
	/*				Main Window 								*/
	protected Shell	shlIxiDataManger;
	private Text destinationText;
	private Text incrementText;
	private Text sourceText;
	private Text titleText;
	private final FormToolkit formToolkit = new FormToolkit(Display.getDefault());

	// Dialog
	protected Shell ixiDialogShell;
	public String Info;
	
	// ----------------------------------------------------------------------------
	// Test Area. 
	
	// Test Dialog
	protected Shell ixiConfigShell;

	
	public void ConfigurationWindow()
    {
		Display display = Display.getDefault();
	
		ixiConfigShell = new Shell(display, SWT.ON_TOP | SWT.SYSTEM_MODAL);
	
		
	    System.out.println("ConfigArea");
	
    }
	
	
	
	
	// END TEST AREA
	// ----------------------------------------------------------------------------
	
	public void DialogWindow( boolean getSource )
    {
		Display display = Display.getDefault();
	
	    ixiDialogShell = new Shell(display, SWT.ON_TOP | SWT.SYSTEM_MODAL);
	    DirectoryDialog dialog = new DirectoryDialog(ixiDialogShell);
	    dialog.setFilterPath( ixiEngine.workingDir );
	    dialog.setText(" Directory Selection ");
	    
	    String temp = dialog.open();
	    
	    if ( temp != null )
    	{
		    if ( getSource )
		    {
		    	sourceText.setText(temp);
		    }
		    else
		    {
		    	destinationText.setText(temp);
		    }
    	}
    }
	
	
	/**
	 * Open the window.
	 * @wbp.parser.entryPoint
	 */
	public void open()
	{	
		// ----------------------------
		
		
		//ConfigurationWindow();
		
		
		//----------------------------
		
		
		
		CreateNewBackup = false;
		cal.setTime(ixiInitiator.CurrentDate);
		Display display = Display.getDefault();
		createContents();
		shlIxiDataManger.open();
		shlIxiDataManger.layout();
		while (!shlIxiDataManger.isDisposed())
		{	
			if (!display.readAndDispatch())
			{
				display.sleep();
			}
		}
	}

	/**
	 * Create contents of the window.
	 */
	
	protected void setScheduleList( List sl )
	{
		sl.removeAll();
		String[] BackupLabels = ixiInitiator.MainEngine.getListOfBackups();
		
		for( int i = 0; i < BackupLabels.length; i++)
		{
			sl.add( BackupLabels[i] );
		}
	}
	
	protected void createContents()
	{	
		/*
		 * 
		 * 						Contents						
		 * 
		 * 
		 * */
		shlIxiDataManger = new Shell();
		shlIxiDataManger.setMinimumSize(new Point(384, 480));
		shlIxiDataManger.setSize(616, 443);
		shlIxiDataManger.setText("Data Manger");
		shlIxiDataManger.setLayout(null);
		
		titleText = new Text(shlIxiDataManger, SWT.BORDER);
		titleText.setEditable(false);
		titleText.setEnabled(false);
		titleText.setBounds(10, 42, 216, 28);
		formToolkit.adapt(titleText, true, true);
		titleText.setText(" ");
		
		sourceText = new Text(shlIxiDataManger, SWT.BORDER);
		sourceText.setBounds(10, 100, 216, 28);
		sourceText.setEnabled(false);
		sourceText.setEditable(false);
		sourceText.setText(" ");
		
		destinationText = new Text(shlIxiDataManger, SWT.BORDER);
		destinationText.setBounds(10, 158, 216, 28);
		destinationText.setEditable(false);
		destinationText.setEnabled(false);
		destinationText.setText(" ");
		
		incrementText = new Text(shlIxiDataManger, SWT.BORDER);
		incrementText.setBounds(182, 318, 44, 35);
		incrementText.setEnabled(false);
		incrementText.setEditable(false);
		incrementText.setText(" ");

		Label lblTitle_1 = formToolkit.createLabel(shlIxiDataManger, "Title", SWT.NONE);
		lblTitle_1.setBackground(SWTResourceManager.getColor(SWT.COLOR_WIDGET_BACKGROUND));
		lblTitle_1.setBounds(10, 21, 81, 18);
		
		Label lblSource = new Label(shlIxiDataManger, SWT.NONE);
		lblSource.setBounds(10, 76, 81, 18);
		lblSource.setText("Source : ");
		
		Label lblDestination = new Label(shlIxiDataManger, SWT.NONE);
		lblDestination.setBounds(10, 134, 131, 18);
		lblDestination.setText("Destination : ");
		
		Label lblUpdateIncrement = new Label(shlIxiDataManger, SWT.NONE);
		lblUpdateIncrement.setBounds(10, 318, 166, 18);
		lblUpdateIncrement.setText("Update Increment :");

		Label lblBackupDateScheduled = new Label(shlIxiDataManger, SWT.NONE);
		lblBackupDateScheduled.setBounds(10, 192, 129, 18);
		lblBackupDateScheduled.setText("Backup Date : ");
		
		Label lblBackupsScheduled = new Label(shlIxiDataManger, SWT.NONE);
		lblBackupsScheduled.setBounds(368, 21, 179, 18);
		lblBackupsScheduled.setText("Backups Scheduled :");
		
		Label lblHour = formToolkit.createLabel(shlIxiDataManger, "Hour :", SWT.NONE);
		lblHour.setBackground(SWTResourceManager.getColor(SWT.COLOR_WIDGET_BACKGROUND));
		lblHour.setBounds(10, 251, 72, 18);
		
		Label lblMinute = formToolkit.createLabel(shlIxiDataManger, "Minute :", SWT.NONE);
		lblMinute.setBackground(SWTResourceManager.getColor(SWT.COLOR_WIDGET_BACKGROUND));
		lblMinute.setBounds(88, 251, 81, 18);
		
		final List list = new List(shlIxiDataManger, SWT.BORDER);
		list.setBounds(368, 42, 210, 234);
		
		final DateTime dateChooser = new DateTime(shlIxiDataManger, SWT.BORDER);
		dateChooser.setBounds(10, 216, 151, 29);
		dateChooser.setEnabled(false);
		dateChooser.setForeground(SWTResourceManager.getColor(SWT.COLOR_WIDGET_DARK_SHADOW));
		
		final Button radioButtonDays = new Button(shlIxiDataManger, SWT.RADIO);
		radioButtonDays.setBounds(97, 347, 72, 24);
		radioButtonDays.setEnabled(false);
		radioButtonDays.setText("Days");
		
		final Button radioButtonHours = new Button(shlIxiDataManger, SWT.RADIO);
		radioButtonHours.setBounds(10, 347, 88, 24);
		radioButtonHours.setEnabled(false);
		radioButtonHours.setText("Hours");
		
		final Button radioButtonWeeks = new Button(shlIxiDataManger, SWT.RADIO);
		radioButtonWeeks.setBounds(10, 377, 81, 24);
		radioButtonWeeks.setEnabled(false);
		radioButtonWeeks.setText("Weeks");
		
		final Button radioButtonMonths = new Button(shlIxiDataManger, SWT.RADIO);
		radioButtonMonths.setBounds(97, 377, 82, 24);
		radioButtonMonths.setEnabled(false);
		radioButtonMonths.setText("Months");
		
		final Button btnNew = formToolkit.createButton(shlIxiDataManger, "New", SWT.NONE);
		final Button btnCancel = new Button(shlIxiDataManger, SWT.NONE);
		final Button btnLoad = new Button(shlIxiDataManger, SWT.NONE);
		final Button btnSave = new Button(shlIxiDataManger, SWT.NONE);
		final Button btnEditSource = new Button(shlIxiDataManger, SWT.NONE);
		Button btnEditIncrement = new Button(shlIxiDataManger, SWT.NONE);
		Button btnEditDestination = new Button(shlIxiDataManger, SWT.NONE);
		Button btnEditTitle = new Button(shlIxiDataManger, SWT.NONE);
		Button btnEditDate = new Button(shlIxiDataManger, SWT.NONE);
		btnCancel.setBounds(368, 282, 102, 30);
		
		final Combo comboHour = new Combo(shlIxiDataManger, SWT.NONE);
		comboHour.setBounds(10, 282, 72, 30);
		formToolkit.adapt(comboHour);
		formToolkit.paintBordersFor(comboHour);
		comboHour.setEnabled(false);
		
		final Combo comboMinute = new Combo(shlIxiDataManger, SWT.NONE);
		comboMinute.setBounds(88, 282, 73, 30);
		formToolkit.adapt(comboMinute);
		formToolkit.paintBordersFor(comboMinute);
		comboMinute.setEnabled(false);
		
		/*
		 * 
		 * 
		 * 					Create / Populate Backup list and Combo boxes						
		 * 
		 * 
		 * */
		
		setScheduleList( list );

		for( int i = 0; i < 60; i++) 
		{
			if ( i < 10 )
				comboMinute.add( "0" + Integer.toString(i) );
			else
				comboMinute.add( Integer.toString(i) );
		}
		comboMinute.select(0);
	
		for( int i = 1; i < 25; i++) 
		{
			if ( i < 10 )
				comboHour.add( "0" + Integer.toString(i) );
			else
				comboHour.add( Integer.toString(i) );
		}
		comboHour.select(0);
	
	   /*
		* 
		*				BUTTON : SAVE BUTTON
		*
		*
		*/
		btnSave.setBounds(476, 282, 102, 30);
		btnSave.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if( CurrentJob != null && !"".equals( titleText.getText().trim() ) )
				{
					CurrentJob.Title = titleText.getText().trim();
					
					if( !" ".equals( sourceText.getText() ) && !" ".equals( destinationText.getText() ) )
					{
						CurrentJob.Source = sourceText.getText();
						CurrentJob.Destination = destinationText.getText();

						if( ixiInitiator.MainEngine.isNumeric( incrementText.getText().trim() ) )
						{
							CurrentJob.UpdateIncrement = Integer.parseInt(incrementText.getText().trim());;

							if( radioButtonDays.getSelection() )
							{
								CurrentJob.UpdateUnit = 1;
							}
							else if( radioButtonHours.getSelection() )
							{
								CurrentJob.UpdateUnit = 2;
							}
							else if( radioButtonWeeks.getSelection() )
							{
								CurrentJob.UpdateUnit = 3;
							}
							else if( radioButtonMonths.getSelection() )
							{
								CurrentJob.UpdateUnit = 4;
							}
							else
							{
								ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Please make sure an Update Unit is selected.", "MAIN_WINDOW_SAVE_BUTTON");
							}
		
							if ( ixiInitiator.MainEngine.isNumeric( comboMinute.getText().trim() ) )
							{	
								if ( ixiInitiator.MainEngine.isNumeric( comboHour.getText().trim() ) )
								{
									
									String NextUpdateString = ( Integer.toString(dateChooser.getYear()).trim() + "-" );
									NextUpdateString += ( Integer.toString(dateChooser.getMonth()+1 ).trim() + "-" );
									NextUpdateString += ( Integer.toString(dateChooser.getDay() ).trim() + "." );
									NextUpdateString += ( comboHour.getText().trim() + "-" );
									NextUpdateString += ( comboMinute.getText().trim() + "-00" );
									NextUpdateString = NextUpdateString.trim();
								
									try
									{
										CurrentJob.NextUpdate = dateFormatter.parse( NextUpdateString.trim() );
									}
									catch (ParseException e1)
									{
										e1.printStackTrace();
									}
									
									if ( CreateNewBackup )
									{
										CurrentJob.BackupId = createNewJobReturnID();
										CreateNewBackup = false;
									}
									
									ixiInitiator.MainEngine.setBackupByID(CurrentJob.BackupId, CurrentJob);
									
									ixiInitiator.MainEngine.updateScheduleFile();
									
									setScheduleList( list );
									
									resetAllValues(dateChooser, radioButtonDays, radioButtonHours, radioButtonWeeks, radioButtonMonths, comboHour, comboMinute, btnNew, btnLoad, btnSave, btnCancel);
									
								}
								else
								{
									ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Hour Selected", "MAIN_WINDOW_SAVE_BUTTON");
									
								}
							}
							else 
							{
								ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Minute Selected", "MAIN_WINDOW_SAVE_BUTTON");
							}
						}
						else
						{
							ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Update Increment", "MAIN_WINDOW_SAVE_BUTTON");
						}
					}
					else
					{
						if ( "".equals( sourceText.getText().trim() ) ) 
						{
							ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Source ", "MAIN_WINDOW_SAVE_BUTTON");
						}
						else
						{
							ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Destination", "MAIN_WINDOW_SAVE_BUTTON");
						}
					}
				}
				else
				{
					if ( " ".equals( titleText.getText().trim() ) ) 
					{
						ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Title", "MAIN_WINDOW_SAVE_BUTTON");
					}
					else
					{
						ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid Job", "MAIN_WINDOW_SAVE_BUTTON");
					}
				}
			}
		});
		btnSave.setEnabled(false);
		btnSave.setText("Save");
		
	   /*
		* 
		*				BUTTON : EDIT TITLE
		*
		*
		*/
		btnEditTitle.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e) 
			{	
				if ( CurrentJob != null )
				{
					btnSave.setEnabled(true);
					titleText.setEnabled(true);
					titleText.setEditable(true);
				}
			}
		});
		btnEditTitle.setBounds(232, 40, 102, 30);
		formToolkit.adapt(btnEditTitle, true, true);
		btnEditTitle.setText("Edit");
		
	   /*
		* 
		*				BUTTON : EDIT SOURCE
		*
		*
		*/
		btnEditSource.setBounds(232, 98, 102, 30);
		btnEditSource.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if ( CurrentJob != null )
				{
					btnSave.setEnabled(true);
					sourceText.setEnabled(true);
					DialogWindow( true );
				
				}
			}
		});
		btnEditSource.setText("Edit");
		
	   /*
		* 
		*				BUTTON : EDIT DESTINATION
		*
		*
		*/
		btnEditDestination.setBounds(232, 156, 102, 30);
		btnEditDestination.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if ( CurrentJob != null )
				{
					btnSave.setEnabled(true);
					destinationText.setEnabled(true);
					DialogWindow( false );
				}
			}
		});
		btnEditDestination.setText("Edit");
		
	   /*
		* 
		*				BUTTON : EDIT DATE
		*
		*
		*/
		btnEditDate.setBounds(232, 215, 102, 30);
		btnEditDate.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if ( CurrentJob != null )
				{
					btnSave.setEnabled(true);
					dateChooser.setEnabled(true);
					comboMinute.setEnabled(true);
					comboHour.setEnabled(true);
				}
			}
		});
		btnEditDate.setText("Edit");

	   /*
		* 
		*				BUTTON : EDIT INCREMENT
		*
		*
		*/
		btnEditIncrement.setBounds(232, 318, 102, 30);
		btnEditIncrement.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if ( CurrentJob != null )
				{
					btnSave.setEnabled(true);
					incrementText.setEnabled(true);
					incrementText.setEditable(true);
					radioButtonDays.setEnabled(true);
					radioButtonHours.setEnabled(true);
					radioButtonWeeks.setEnabled(true);
					radioButtonMonths.setEnabled(true);
				}
			}
		});
		btnEditIncrement.setText("Edit");
		
	   /*
		* 
		*				BUTTON : LOAD
		*
		*
		*/
		btnLoad.setBounds(368, 318, 102, 30);
		btnLoad.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{	
				if ( -1 != list.getSelectionIndex() )
				{
					btnCancel.setEnabled(true);
					btnNew.setEnabled(false);
					
					String selection = list.getItem(  list.getSelectionIndex() );
					String[] exploded = selection.split(":");
					
					// Load the current Job
					CurrentJob = ixiInitiator.MainEngine.getBackupByID(Integer.parseInt(exploded[0] ));
					
					// Set window objects to display job information
					titleText.setText(CurrentJob.Title);
					sourceText.setText( CurrentJob.Source );
					destinationText.setText( CurrentJob.Destination );
					incrementText.setText( Integer.toString( CurrentJob.UpdateIncrement ) );
					Calendar lcal = Calendar.getInstance();
					lcal.setTime(CurrentJob.NextUpdate);
					dateChooser.setDate(lcal.get(Calendar.YEAR), lcal.get(Calendar.MONTH), lcal.get(Calendar.DAY_OF_MONTH));
					comboHour.select( lcal.get(Calendar.HOUR)-1 );
					comboMinute.select( lcal.get(Calendar.MINUTE)-1 );	
			
					
					switch ( CurrentJob.UpdateUnit )
					{
					case 0:
						ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "Invalid UpdateUnit", "MAIN_WINDOW_LOAD_BUTTON");
						break;
					case 1:
						radioButtonDays.setSelection(true);
						break;
					case 2:
						radioButtonHours.setSelection(true);
						break;
					case 3:
						radioButtonWeeks.setSelection(true);
						break;
					case 4:
						radioButtonMonths.setSelection(true);
						break;
					default:
						ixiInitiator.MainEngine.ixiErrorHandler.ReportError(1, "DEFAULT_ACCESSED", "MAIN_WINDOW_LOAD_BUTTON");
						System.exit(0);
						break;
					}
				}
				else if ( -1 == list.getSelectionIndex() )
				{
					ixiInitiator.MainEngine.ixiErrorHandler.ReportError(9, "No backup selected", "MAIN_WINDOW_LOAD_BUTTON");
				}
			}
		});
		btnLoad.setText("Load");
		
	   /*
		* 
		*				BUTTON : CANCEL
		*
		*
		*/
		btnCancel.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{

				resetAllValues(dateChooser, radioButtonDays, radioButtonHours, radioButtonWeeks, radioButtonMonths, comboHour, comboMinute, btnNew, btnLoad, btnSave, btnCancel);
			}
		});
		btnCancel.setEnabled(false);
		btnCancel.setText("Cancel");
		
	   /*
		* 
		*				BUTTON : NEW BACKUP
		*
		*
		*/
		btnNew.addMouseListener(new MouseAdapter() 
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if ( CurrentJob == null )
				{
					CreateNewBackup = true;
					CurrentJob =  new ixi.ixiEngine.Backup();
					
					btnCancel.setEnabled(true);
					btnSave.setEnabled(true);
					
					titleText.setEditable(true);
					titleText.setEnabled(true);
					titleText.forceFocus();
				}
				
			}
		});
		btnNew.setBounds(476, 318, 102, 30);
		
	   /*
		* 
		*				BUTTON : DELETE
		*
		*
		*/
		Button btnDelete = new Button(shlIxiDataManger, SWT.NONE);
		btnDelete.addMouseListener(new MouseAdapter()
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				if ( CurrentJob != null )
				{
					CurrentJob.Title = "[$!~REMV_***]";
					ixiInitiator.MainEngine.setBackupByID(CurrentJob.BackupId, CurrentJob);
					ixiInitiator.MainEngine.updateScheduleFile();
					
					setScheduleList( list );
					
					resetAllValues(dateChooser, radioButtonDays, radioButtonHours, radioButtonWeeks, radioButtonMonths, comboHour, comboMinute, btnNew, btnLoad, btnSave, btnCancel);
					
				}
			}
		});
		btnDelete.setBounds(476, 371, 102, 30);
		formToolkit.adapt(btnDelete, true, true);
		btnDelete.setText("Delete");

		shlIxiDataManger.setTabList(new Control[]{btnLoad, btnEditSource, btnEditDestination, btnEditDate, btnEditIncrement, incrementText, radioButtonDays, radioButtonHours, radioButtonWeeks, list});
	}
	
	public void resetAllValues(DateTime dateChooser, Button radioButtonDays, Button radioButtonHours, Button radioButtonWeeks, Button radioButtonMonths, Combo comboHour, Combo comboMinute, Button btnNew, Button btnLoad, Button btnSave, Button btnCancel)
	{
		// Reset initial object values
		titleText.setText(" ");
		sourceText.setText(" ");
		destinationText.setText(" ");
		incrementText.setText(" ");
		dateChooser.setDate(cal.get(Calendar.YEAR), cal.get(Calendar.MONTH), cal.get(Calendar.DAY_OF_MONTH));
		radioButtonDays.setSelection(false);
		radioButtonHours.setSelection(false);
		radioButtonWeeks.setSelection(false);
		radioButtonMonths.setEnabled(false);
		
		// Disable all initially disabled objects
		titleText.setEnabled(false);
		sourceText.setEnabled(false);
		destinationText.setEnabled(false);
		incrementText.setEnabled(false);
		dateChooser.setEnabled(false);
		radioButtonDays.setEnabled(false);
		radioButtonHours.setEnabled(false);
		radioButtonWeeks.setEnabled(false);
		radioButtonMonths.setEnabled(false);
		comboHour.setEnabled(false);
		comboMinute.setEnabled(false);
		comboHour.select(0);
		comboMinute.select(0);
		
		// Clear the current Job
		CurrentJob = null;

		// Enable New, and focus on Load
		btnLoad.forceFocus();
		btnNew.setEnabled(true);
		btnSave.setEnabled(false);
		btnCancel.setEnabled(false);
	}
}
