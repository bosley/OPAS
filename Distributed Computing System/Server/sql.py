import MySQLdb
import datetime

sql_settings = {
	"ADDR": "127.0.0.1",
	"USER": "USERNAME_HERE",
	"PASS": "PASSWORD_HERE",
	"DB": "DB_NAME_HERE" 
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
		reuslts = cursor.fetchall()
	except:
		results = -1
	db.close()
	return results
