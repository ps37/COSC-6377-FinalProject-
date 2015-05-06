# Import smtplib for the actual sending function
import smtplib
import datetime

# Import the email modules we'll need
from email.mime.text import MIMEText

# Open a plain text file for reading.  For this example, assume that
# the text file contains only ASCII characters.
textfile = 'measurements.txt'
with open(textfile) as fp:
    # Create a text/plain message
    msg = MIMEText(fp.read())

# me == the sender's email address
# you == the recipient's email address

msg['Subject'] = 'The contents of %s' % textfile

smtp = smtplib.SMTP_SSL()
#smtp.set_debuglevel(debuglevel)
smtp.connect('smtp.gmail.com', 465)
smtp.login('sprudhvichandra@gmail.com', "Qwerty12#")

#msg = "\nHello!"
# Send the message via our own SMTP server.
#server = smtplib.SMTP('smtp.gmail.com', 587)
#server.login("prudhvi.af121@gmail.com", "sIMHADRI1992#")
smtp.sendmail('sprudhvichandra@gmail.com', 'sprudhvichandra@gmail.com', msg.as_string())
smtp.quit()