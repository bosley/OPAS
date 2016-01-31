/*
 *				Author: Josh A. Bosley 
 * 				 Class: CSCI 434
 * 				  Date: January/15/16
 * 			Assignment: 2
 * 		   Description: Java program to compute the square roots of
 * 						a quadratic equation. Reads A B C constants from file prob2.in
 * 						Determines number of solutions, and calculates them. 
 * 						Outputs to prob2.out
 */

package assign2;

import java.io.*;
import java.util.*;

public class assign2
{
	public static String outTxt, nln, inloc, oloc;
	
	public static void analyze(List <Integer> nums)
	{
		// Coefficients for the sake of demonstration
		int a = nums.get(0), 
				b = nums.get(1), 
				c = nums.get(2);

		//b^2-4ac
		int s = b*b-4*a*c;

		// For formatting output
		String ot = "", ot2 = ot;
		if(b > 0)	ot2 = "X^2+";	
		else		ot2 = "X^2";
		
		if(c > 0)	ot = "X+";
		else		ot = "X";
		
		outTxt += (a + ot2 + b + ot + c + "=0 has ");
		
		// Check sign for calculation
		if(s < 0)
		{
			// No solutions
			outTxt += ("no real roots" + nln);
		}
		else if( s == 0 )
		{
			// Put together each peace of formula, and calculate X
			int a2 = a*2, negb = b;
			if(b > 0) negb = b-2*b;
			double sqs = Math.sqrt(s),
					numpos = negb + sqs,
					X1 = numpos/a2;
			
			// Format X for proper output
			String SX1 = String.format("%.02f", X1);
			
			// Show X
			outTxt += ("exactly one real root: X = ");
			outTxt += (SX1 + nln);
		}
		else
		{
			// Put together each peace of formula, and calculate Xs 
			int a2 = a*2, negb = b;
			if(b > 0) negb = b-2*b;
			double sqs = Math.sqrt(s),
					numpos = negb + sqs,
					numneg = negb - sqs,
					X1 = numpos/a2,
					X2 = numneg/a2;

			// Format Xs for proper output
			String SX1 = String.format("%.02f", X1),
					SX2 = String.format("%.02f", X2);
			
			// Show both Xs
			outTxt += ("two real roots: X1 = ");
			outTxt += (SX1);
			outTxt += (" and X2 = ");
			outTxt += (SX2 + nln);
		}
	}
	
	public static void main(String[] args)
	{
		// Make a list for the numbers, and a make a file object for input
		nln = "\n";
		outTxt = "";
		inloc = "./prob2.in";
		oloc = "./prob2.out";
	
		// Change newline, and input location if on windows. :(
		if(System.getProperty("os.name").startsWith("Windows"))
		{
			nln = "\r\n";
			inloc = "E:\\eclipse\\workspace\\assign2\\assign2\\prob2.in";
			oloc = "E:\\eclipse\\workspace\\assign2\\assign2\\prob2.out";
		}
		
		// Try opening the file and building t
		List<Integer> numbers = new ArrayList<>();
		File prob2In = new File(inloc);
		try
		{
			Scanner sc = new Scanner(prob2In);
			while(sc.hasNextInt())
			{
				numbers.add(sc.nextInt());
			}
			sc.close();
		}
		catch (FileNotFoundException e)
		{
			e.printStackTrace();
		}
	    
		// Make sure nothing silly is going on at the end of the file
	    if( 	(numbers.get(numbers.size()-1) != 0) || 
	    		(numbers.get(numbers.size()-2) != 0) || 
	    		(numbers.get(numbers.size()-3) != 0) )
	    {
	    	System.out.println("Invalid file input.");
	    	System.exit(0);
	    }
	    
	    // Get every three numbers, and send off to be analyzed
	    List <Integer> nums = new ArrayList<>();
	    for(int i = 0; i  < (numbers.size() - 3); i++)
	    {
	        if((i+1) % 3 == 0)
	        {
	        	nums.add(numbers.get(i));
				analyze(nums);
	        	nums.clear();
	        }
	        else
	        {
	        	nums.add(numbers.get(i));
	        }
	    }

		// Setup output 
		File prob2out = new File(oloc);
		try
		{
			PrintWriter outp = new PrintWriter(prob2out);
			outp.write(outTxt);
			outp.close();
		}
		catch (FileNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
