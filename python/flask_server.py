# A file to host a basic local Flask dashboard for editing res/data.csv.
# The app backs up the CSV on load and writes changes atomically.

import csv
import datetime
import os
import threading
import signal

import subprocess
import re

from functools import wraps

from flask import Flask, jsonify, request, render_template, send_file, session, redirect, url_for, flash
from werkzeug.security import check_password_hash, generate_password_hash

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.normpath(os.path.join(SCRIPT_DIR, "../res"))
TEMPLATE_DIR = os.path.normpath(os.path.join(SCRIPT_DIR, "../res/html"))
DATA_PATH = os.path.join(DATA_DIR, "data.csv")
BACKUP_DIR = os.path.join(DATA_DIR, "backups")

os.makedirs(BACKUP_DIR, exist_ok=True)

app = Flask(__name__, template_folder=TEMPLATE_DIR)
app.secret_key = 'aqua-bar-secure-session-key-2024-change-this-randomly'  # CHANGE THIS TO A RANDOM STRING
file_lock = threading.Lock()

with open('/home/piaqua/Desktop/AquaBar/res/ReloadSignal.pid') as f:
        CPP_PID = int(f.read())

# Password configuration
# Create a file named config.py in the same directory as this script.
# Add: PASSWORD_HASH = generate_password_hash('your_chosen_password')
# Then add config.py to .gitignore to avoid committing it to git.
# Example: from werkzeug.security import generate_password_hash
#          PASSWORD_HASH = generate_password_hash('mypassword')
try:
    import config
    PASSWORD_HASH = config.PASSWORD_HASH
except ImportError:
    # Fallback for development - CHANGE THIS IN PRODUCTION
    PASSWORD_HASH = generate_password_hash('default_password')

def login_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if 'logged_in' not in session:
            return redirect(url_for('login'))
        return f(*args, **kwargs)
    return decorated_function


@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        password = request.form.get('password')
        if password and check_password_hash(PASSWORD_HASH, password):
            session['logged_in'] = True
            return redirect(url_for('index'))
        else:
            flash('Ugyldig passord')
    return render_template('login.html')


@app.route('/logout')
@login_required
def logout():
    session.pop('logged_in', None)
    return redirect(url_for('login'))


def normalize_user_row(row):
    if len(row) < 3:
        return None
    name = row[0].strip()
    rfid = row[1].strip()
    spent_value = row[2].strip()
    try:
        spent = int(spent_value)
    except ValueError:
        spent = 0

    blocked = False
    if len(row) >= 4:
        blocked_value = str(row[3]).strip().lower()
        blocked = blocked_value in ("1", "true", "yes", "blocked", "x")

    return {
        "name": name,
        "rfid": rfid,
        "spent": spent,
        "blocked": blocked,
    }


def read_users():
    if not os.path.exists(DATA_PATH):
        raise FileNotFoundError(f"Missing data file: {DATA_PATH}")

    users = []
    with open(DATA_PATH, newline="", encoding="utf-8") as csv_file:
        reader = csv.reader(csv_file)
        for row in reader:
            user = normalize_user_row(row)
            if user is not None:
                users.append(user)
    return users


def backup_data():
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H:%M")
    backup_path = os.path.join(BACKUP_DIR, f"Backup_{timestamp}.csv")
    with open(DATA_PATH, "r", encoding="utf-8") as source:
        content = source.read()
    with open(backup_path, "w", encoding="utf-8", newline="") as backup_file:
        backup_file.write(content)
    return backup_path


def write_users(users):
    temp_path = DATA_PATH + ".tmp"
    with open(temp_path, "w", encoding="utf-8", newline="") as csv_file:
        writer = csv.writer(csv_file)
        for user in users:
            writer.writerow([
                user["name"],
                user["rfid"],
                str(user.get("spent", 0)),
                "1" if user.get("blocked") else "0",
            ])
    os.replace(temp_path, DATA_PATH)


def build_stats(users):
    total_users = len(users)
    blocked_users = sum(1 for user in users if user.get("blocked"))
    total_spent = sum(user.get("spent", 0) for user in users)
    return {
        "total_users": total_users,
        "total_spent": total_spent,
        "blocked": blocked_users,
    }


@app.route("/")
@login_required
def index():
    return render_template("dashboard.html")


@app.route("/load", methods=["POST"])
@login_required
def load_data():
    with file_lock:
        backup_path = backup_data()
        users = read_users()
    return jsonify({
        "loaded": True,
        "backup": os.path.basename(backup_path),
        "users": users,
        "stats": build_stats(users),
    })


@app.route("/save", methods=["POST"])
@login_required
def save_data():
    payload = request.get_json(force=True)

    os.kill(CPP_PID, signal.SIGUSR1)

    if isinstance(payload, dict) and "users" in payload:
        raw_users = payload["users"]
        if not isinstance(raw_users, list):
            return jsonify({"error": "Expected users list."}), 400

        users = []
        for raw in raw_users:
            if not isinstance(raw, dict):
                continue
            name = str(raw.get("name", "")).strip()
            rfid = str(raw.get("rfid", "")).strip()
            if not name or not rfid:
                continue

            spent_value = raw.get("spent", 0)
            try:
                spent = int(spent_value)
            except (ValueError, TypeError):
                spent = 0

            users.append({
                "name": name,
                "rfid": rfid,
                "spent": spent,
                "blocked": bool(raw.get("blocked", False)),
            })

        if not users:
            return jsonify({"error": "No valid users to save."}), 400

        with file_lock:
            write_users(users)
        return jsonify({"saved": True, "stats": build_stats(users)})

    if not isinstance(payload, list):
        return jsonify({"error": "Expected a list of user updates."}), 400

    with file_lock:
        users = read_users()
        for update in payload:
            if not isinstance(update, dict):
                continue
            index = update.get("index")
            if not isinstance(index, int) or index < 0 or index >= len(users):
                continue
            users[index]["blocked"] = bool(update.get("blocked", False))
        write_users(users)


    return jsonify({"saved": True, "stats": build_stats(users)})


@app.route("/download")
@login_required
def download_csv():
    if not os.path.exists(DATA_PATH):
        return jsonify({"error": "Data file not found."}), 404
    return send_file(DATA_PATH, as_attachment=True, mimetype="text/csv")


@app.route("/backups", methods=["GET"])
@login_required
def list_backups():
    files = sorted(
        (f for f in os.listdir(BACKUP_DIR) if f.endswith(".csv")),
        reverse=True,
    )
    return jsonify({"backups": files})


@app.route("/backups/<filename>", methods=["GET"])
@login_required
def load_backup(filename):
    if filename != os.path.basename(filename):
        return jsonify({"error": "Invalid filename"}), 400
    path = os.path.join(BACKUP_DIR, filename)
    if not os.path.exists(path):
        return jsonify({"error": "Backup not found"}), 404
    users = []
    with open(path, newline="", encoding="utf-8") as csv_file:
        reader = csv.reader(csv_file)
        for row in reader:
            user = normalize_user_row(row)
            if user is not None:
                users.append(user)
    return jsonify({"users": users, "stats": build_stats(users)})


@app.route("/backups/<filename>", methods=["DELETE"])
@login_required
def delete_backup(filename):
    if filename != os.path.basename(filename):
        return jsonify({"error": "Invalid filename"}), 400
    path = os.path.join(BACKUP_DIR, filename)
    if not os.path.exists(path):
        return jsonify({"error": "Backup not found"}), 404
    os.remove(path)
    return jsonify({"deleted": True})


def get_tailscale_ip():
    result = subprocess.run(["tailscale", "ip", "-4"], capture_output=True, text=True)
    ip = result.stdout.strip()
    if re.match(r"100\.\d+\.\d+\.\d+", ip):
        return ip
    raise RuntimeError("Could not get Tailscale IP — is Tailscale running?")


if __name__ == "__main__":
    host = get_tailscale_ip()
    print(f"Flask binding to Tailscale IP: {host}")
    app.run(host=host, port=5000, debug=False, threaded=True)

