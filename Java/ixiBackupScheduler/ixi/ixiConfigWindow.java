package ixi;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.events.MouseAdapter;
import org.eclipse.swt.events.MouseEvent;

public class ixiConfigWindow extends ixiInitiator
{

	protected Shell	ixiConfigShell;

	private ixi.ixiEngine.Config CurrentConfig;

	/**
	 * Open the window.
	 * @wbp.parser.entryPoint
	 */
	public void open()
	{
		Display display = Display.getDefault();
		createContents();
		ixiConfigShell.open();
		ixiConfigShell.layout();
		while (!ixiConfigShell.isDisposed())
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
	protected void createContents()
	{
		ixiConfigShell = new Shell();
		ixiConfigShell.setSize(325, 381);
		ixiConfigShell.setText("Configuration Menu");
		
		Group grpErrorReportingOptions = new Group(ixiConfigShell, SWT.NONE);
		grpErrorReportingOptions.setText("Error Reporting Options");
		grpErrorReportingOptions.setBounds(10, 10, 303, 125);
		
		final Button btnWriteConsole = new Button(grpErrorReportingOptions, SWT.CHECK);
		btnWriteConsole.setBounds(10, 91, 250, 24);
		btnWriteConsole.setText("Write Errors To Console");
		
		Button btnWriteErrorsPop = new Button(grpErrorReportingOptions, SWT.CHECK);
		btnWriteErrorsPop.setBounds(10, 61, 237, 24);
		btnWriteErrorsPop.setText("Write Errors To Pop Up");
		
		final Button btnWriteFile = new Button(grpErrorReportingOptions, SWT.CHECK);
		btnWriteFile.setBounds(10, 34, 224, 24);
		btnWriteFile.setText("Write Errors To File");
		
		Group grpWindowBehavior = new Group(ixiConfigShell, SWT.NONE);
		grpWindowBehavior.setText("Window Behavior");
		grpWindowBehavior.setBounds(10, 160, 303, 97);
		
		final Button btnDisplayBackupWindow = new Button(grpWindowBehavior, SWT.CHECK);
		btnDisplayBackupWindow.setBounds(10, 63, 224, 24);
		btnDisplayBackupWindow.setText("Display Backup Window");
		
		final Button btnAlwaysShowJob = new Button(grpWindowBehavior, SWT.CHECK);
		btnAlwaysShowJob.setBounds(10, 31, 224, 24);
		btnAlwaysShowJob.setText("Always Show Job Editor");
		
		Button btnSave = new Button(ixiConfigShell, SWT.NONE);
		btnSave.setBounds(211, 296, 102, 30);
		btnSave.setText("Save");
		
		// doErrorPopup
		
		// Set Current Configuration to the current loaded configuration.
		CurrentConfig = ixiInitiator.MainEngine.getConfiguration();
		
		if ( CurrentConfig.doSetup == 1 )
		{
			// Once this window is initially displayed, it should not show unless called.
			CurrentConfig.doSetup = 0;
		}
		if( CurrentConfig.doErrorConsole == 1)
		{
			btnWriteConsole.setSelection(true);
		}
		if ( CurrentConfig.doErrorFile == 1 )
		{
			btnWriteFile.setSelection(true);
		}
		if ( CurrentConfig.doShowBackupWindow == 1)
		{
			btnDisplayBackupWindow.setSelection(true);
		}
		if ( CurrentConfig.doShowMainDisplay == 1)
		{
			btnAlwaysShowJob.setSelection(true);
		}
		if ( CurrentConfig.doErrorPopup == 1)
		{
			btnWriteErrorsPop.setSelection(true);
		}
		
		/*
		 * 
		 * 			SAVE BUTTON
		 * 
		 * */
		btnSave.addMouseListener(new MouseAdapter()
		{
			@Override
			public void mouseDown(MouseEvent e)
			{
				// Write to file
				if ( btnWriteFile.getSelection() )
				{
					CurrentConfig.doErrorFile = 1;
				}
				else
				{
					CurrentConfig.doErrorFile = -1;
				}
				
				// Write to console
				if ( btnWriteConsole.getSelection() )
				{
					CurrentConfig.doErrorConsole = 1;
				}
				else
				{
					CurrentConfig.doErrorConsole = 0;
				}
				
				// Show backup window
				if ( btnDisplayBackupWindow.getSelection() )
				{
					CurrentConfig.doShowBackupWindow = 1;
				}
				else
				{
					CurrentConfig.doShowBackupWindow = 0;
				}
				
				// Display main window
				if ( btnAlwaysShowJob.getSelection() )
				{
					CurrentConfig.doShowMainDisplay = 1;
				}
				else
				{
					CurrentConfig.doShowMainDisplay = 0;
				}
				
				// Send to engine, and write.
				ixiInitiator.MainEngine.setConfiguration( CurrentConfig );
				ixiInitiator.MainEngine.updateConfigFile();
			}
		});
		

	}
}
