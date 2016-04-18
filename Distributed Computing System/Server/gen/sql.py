import MySQLdb
import datetime

sql_settings = {
	"ADDR": "ADDR",
	"USER": "USER",
	"PASS": "PASS",
	"DB": "DBNAME" 
}

def update(query):
	db = MySQLdb.connect(sql_settings["ADDR"], 
				sql_settings["USER"], 
				sql_settings["PASS"], 
				sql_settings["DB"])
 	return_code = 1
	cursor = db.cursor()
	try:
		cursor.execute(query)
		db.commit()
	except:
		db.rollback()
		return_code = -1
		db.close()
	return return_code

def fetch(query):
	db = MySQLdb.connect(sql_settings["ADDR"], 
				sql_settings["USER"], 
				sql_settings["PASS"], 
				sql_settings["DB"])
	cursor = db.cursor()
	try:
		cursor.execute(query)
		results = cursor.fetchall()
	except:
		results = -1
	db.close()
	return results
