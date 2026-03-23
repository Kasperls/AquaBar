import smtplib
from email.mime.text import MIMEText
import sys
import time
import os


def send_email(subject, body, to_address):
    from_address = "kasperls004@gmail.com"
    password = "fzlp cfoa btyp nzrk"

    mail_body = "Dette er en automatisk mail sendt fra Aqua Bar:\n\n" + body + "\n\nVennlig hilsen,\nAqua Bar Systemet"

    msg = MIMEText(mail_body)
    msg["Subject"] = subject
    msg["From"] = from_address
    msg["To"] = to_address

    with smtplib.SMTP_SSL("smtp.gmail.com", 465) as server:
        server.login(from_address, password)
        server.sendmail(from_address, to_address, msg.as_string())
    

# sys.argv[0] is the script name, so arguments start at [1]
subject = sys.argv[1]
body = sys.argv[2]
to_address = sys.argv[3]

send_email(subject, body, to_address)