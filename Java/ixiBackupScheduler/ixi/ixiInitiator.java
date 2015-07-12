package ixi;

public class ixiInitiator extends ixiEngine
{	
	// Declare main engine
	static ixiEngine MainEngine;
	// Main Window
	public static ixiMain MainWindow; 
	// Configuration Window
	public static ixiConfigWindow ConfigWindow;
	
	public static void main(String[] args)
	{		
		
		MainEngine = new ixiEngine();
		
		// Check to see if setup needs to be done.
		if ( 1 == MainEngine.CurrentConfig.doSetup )
		{
			ixiDisplayConfigWindow();
		}
		// If Program is launched by the user
		if ( "user".equals( args[0]) )
		{
			initUser();
		}
		// Everything is done.
		else
		{
			System.exit(0);
		}
	}
	
	private static void initUser()
	{
		ixiDisplayMainWindow();
	}
	
	private static void ixiDisplayConfigWindow()
	{
		try
		{
			ConfigWindow = new ixiConfigWindow();
			ConfigWindow.open();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	private static void ixiDisplayMainWindow()
	{
		try
		{
			MainWindow = new ixiMain();
			MainWindow.open();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public int createNewJobReturnID()
	{
		return MainEngine.newBackup();
	}
	
}
