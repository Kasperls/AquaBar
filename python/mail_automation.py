import smtplib
from email.mime.text import MIMEText
import sys

def send_email(subject, body, to_address):
    from_address = "kasperls004@gmail.com"
    password = "fzlp cfoa btyp nzrk"

    msg = MIMEText(body)
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