#ifndef WEB_PAGES_H
#define WEB_PAGES_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Attendance System</title>
    <style>
        :root {
            --bg-color: #0b0f19;
            --surface-color: rgba(22, 28, 45, 0.7);
            --border-color: rgba(255, 255, 255, 0.08);
            --text-primary: #f8fafc;
            --text-secondary: #94a3b8;
            --primary: #8b5cf6;
            --primary-hover: #7c3aed;
            --success: #10b981;
            --warning: #f59e0b;
            --danger: #ef4444;
            --info: #06b6d4;
            --font-family: 'Inter', -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }

        body {
            font-family: var(--font-family);
            background-color: var(--bg-color);
            background-image: 
                radial-gradient(at 0% 0%, rgba(139, 92, 246, 0.08) 0px, transparent 50%),
                radial-gradient(at 100% 100%, rgba(6, 182, 212, 0.08) 0px, transparent 50%);
            color: var(--text-primary);
            min-height: 100vh;
            overflow-x: hidden;
            display: flex;
        }

        /* Utilities */
        .flex { display: flex; }
        .align-center { align-items: center; }
        .justify-between { justify-content: space-between; }
        .flex-column { flex-direction: column; }
        .grid { display: grid; }
        .gap-1 { gap: 0.5rem; }
        .gap-2 { gap: 1rem; }
        .gap-3 { gap: 1.5rem; }
        .w-full { width: 100%; }
        .text-center { text-align: center; }

        /* Typography */
        h1, h2, h3, h4 { font-weight: 600; letter-spacing: -0.025em; }
        .text-muted { color: var(--text-secondary); font-size: 0.875rem; }
        .text-xs { font-size: 0.75rem; }
        .text-sm { font-size: 0.875rem; }

        /* Badges */
        .badge {
            display: inline-flex;
            align-items: center;
            padding: 0.25rem 0.625rem;
            border-radius: 9999px;
            font-size: 0.75rem;
            font-weight: 500;
        }
        .badge-success { background-color: rgba(16, 185, 129, 0.15); color: #34d399; }
        .badge-warning { background-color: rgba(245, 158, 11, 0.15); color: #fbbf24; }
        .badge-danger { background-color: rgba(239, 68, 68, 0.15); color: #f87171; }
        .badge-info { background-color: rgba(6, 182, 212, 0.15); color: #22d3ee; }

        /* Sidebar */
        aside {
            width: 260px;
            background-color: rgba(15, 23, 42, 0.8);
            border-right: 1px solid var(--border-color);
            padding: 2rem 1.5rem;
            display: flex;
            flex-direction: column;
            backdrop-filter: blur(10px);
            position: fixed;
            height: 100vh;
            z-index: 100;
            transition: all 0.3s ease;
        }

        .logo {
            display: flex;
            align-items: center;
            gap: 0.75rem;
            margin-bottom: 2.5rem;
            font-size: 1.25rem;
            font-weight: 700;
            color: var(--text-primary);
        }

        .logo svg {
            color: var(--primary);
            width: 2rem;
            height: 2rem;
        }

        .nav-links {
            list-style: none;
            display: flex;
            flex-direction: column;
            gap: 0.5rem;
            flex-grow: 1;
        }

        .nav-link {
            display: flex;
            align-items: center;
            gap: 0.75rem;
            padding: 0.75rem 1rem;
            color: var(--text-secondary);
            text-decoration: none;
            border-radius: 8px;
            font-weight: 500;
            cursor: pointer;
            transition: all 0.2s ease;
        }

        .nav-link:hover, .nav-link.active {
            color: var(--text-primary);
            background-color: rgba(255, 255, 255, 0.05);
        }

        .nav-link.active {
            border-left: 3px solid var(--primary);
            border-radius: 0 8px 8px 0;
            padding-left: calc(1rem - 3px);
            background-color: rgba(139, 92, 246, 0.08);
        }

        .nav-link svg {
            width: 1.25rem;
            height: 1.25rem;
        }

        /* Main Content Area */
        main {
            margin-left: 260px;
            flex-grow: 1;
            padding: 2rem;
            max-width: 1200px;
            width: calc(100% - 260px);
            transition: all 0.3s ease;
        }

        header {
            margin-bottom: 2rem;
        }

        .status-pill {
            display: inline-flex;
            align-items: center;
            gap: 0.375rem;
            background-color: rgba(255, 255, 255, 0.05);
            padding: 0.375rem 0.75rem;
            border-radius: 9999px;
            font-size: 0.875rem;
            border: 1px solid var(--border-color);
        }

        .status-dot {
            width: 8px;
            height: 8px;
            border-radius: 50%;
            background-color: var(--success);
            box-shadow: 0 0 8px var(--success);
        }

        /* Cards & Grids */
        .dashboard-grid {
            grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
            gap: 1.5rem;
            margin-bottom: 2rem;
        }

        .card {
            background-color: var(--surface-color);
            border: 1px solid var(--border-color);
            border-radius: 12px;
            padding: 1.5rem;
            backdrop-filter: blur(8px);
            box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
            transition: transform 0.2s ease, box-shadow 0.2s ease;
        }

        .card:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 30px rgba(0, 0, 0, 0.2);
        }

        .card-title {
            font-size: 0.875rem;
            color: var(--text-secondary);
            margin-bottom: 0.5rem;
            text-transform: uppercase;
            letter-spacing: 0.05em;
        }

        .card-value {
            font-size: 1.75rem;
            font-weight: 700;
        }

        .card-desc {
            margin-top: 0.5rem;
            font-size: 0.75rem;
            color: var(--text-secondary);
        }

        /* Live Scan Panel */
        .live-scan-card {
            grid-column: span 2;
        }
        @media(max-width: 900px) {
            .live-scan-card { grid-column: span 1; }
        }

        .live-notification {
            display: flex;
            align-items: center;
            gap: 1.25rem;
            margin-top: 1rem;
            padding: 1rem;
            border-radius: 8px;
            border-left: 4px solid var(--primary);
            background-color: rgba(255, 255, 255, 0.02);
            animation: pulse-border 2s infinite;
        }

        @keyframes pulse-border {
            0% { border-color: var(--primary); }
            50% { border-color: rgba(139, 92, 246, 0.3); }
            100% { border-color: var(--primary); }
        }

        .live-scan-icon-container {
            width: 48px;
            height: 48px;
            border-radius: 50%;
            display: flex;
            align-items: center;
            justify-content: center;
            background-color: rgba(139, 92, 246, 0.1);
            color: var(--primary);
        }

        .live-scan-icon-container.success { background-color: rgba(16, 185, 129, 0.1); color: var(--success); }
        .live-scan-icon-container.warning { background-color: rgba(245, 158, 11, 0.1); color: var(--warning); }
        .live-scan-icon-container.danger { background-color: rgba(239, 68, 68, 0.1); color: var(--danger); }

        /* Tables */
        .table-container {
            width: 100%;
            overflow-x: auto;
            margin-top: 1.5rem;
            border-radius: 8px;
            border: 1px solid var(--border-color);
        }

        table {
            width: 100%;
            border-collapse: collapse;
            text-align: left;
            font-size: 0.875rem;
        }

        th {
            background-color: rgba(15, 23, 42, 0.6);
            color: var(--text-secondary);
            font-weight: 600;
            padding: 0.75rem 1rem;
            border-bottom: 1px solid var(--border-color);
            text-transform: uppercase;
            font-size: 0.75rem;
            letter-spacing: 0.05em;
        }

        td {
            padding: 1rem;
            border-bottom: 1px solid var(--border-color);
            color: var(--text-primary);
        }

        tr:last-child td {
            border-bottom: none;
        }

        tr:hover td {
            background-color: rgba(255, 255, 255, 0.01);
        }

        /* Buttons & Forms */
        .btn {
            display: inline-flex;
            align-items: center;
            gap: 0.5rem;
            padding: 0.5rem 1rem;
            border-radius: 6px;
            font-size: 0.875rem;
            font-weight: 500;
            cursor: pointer;
            border: 1px solid transparent;
            transition: all 0.2s ease;
            text-decoration: none;
        }

        .btn-primary { background-color: var(--primary); color: white; }
        .btn-primary:hover { background-color: var(--primary-hover); }
        
        .btn-secondary { background-color: rgba(255, 255, 255, 0.05); color: var(--text-primary); border-color: var(--border-color); }
        .btn-secondary:hover { background-color: rgba(255, 255, 255, 0.1); }

        .btn-danger { background-color: rgba(239, 68, 68, 0.1); color: #f87171; border-color: rgba(239, 68, 68, 0.2); }
        .btn-danger:hover { background-color: rgba(239, 68, 68, 0.2); }
        
        .btn-sm { padding: 0.25rem 0.5rem; font-size: 0.75rem; border-radius: 4px; }

        input, select {
            background-color: rgba(15, 23, 42, 0.5);
            border: 1px solid var(--border-color);
            padding: 0.5rem 0.75rem;
            border-radius: 6px;
            color: var(--text-primary);
            font-family: inherit;
            font-size: 0.875rem;
            outline: none;
            transition: border-color 0.2s ease;
        }

        input:focus, select:focus {
            border-color: var(--primary);
        }

        .form-group {
            display: flex;
            flex-direction: column;
            gap: 0.375rem;
            margin-bottom: 1rem;
        }

        .form-row {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 1rem;
        }

        label {
            font-size: 0.75rem;
            font-weight: 600;
            text-transform: uppercase;
            letter-spacing: 0.05em;
            color: var(--text-secondary);
        }

        /* Filter Controls */
        .filter-bar {
            display: flex;
            flex-wrap: wrap;
            gap: 0.75rem;
            margin-bottom: 1.5rem;
            background-color: rgba(15, 23, 42, 0.4);
            padding: 1rem;
            border-radius: 8px;
            border: 1px solid var(--border-color);
        }

        /* Modals */
        .modal-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100vw;
            height: 100vh;
            background-color: rgba(0, 0, 0, 0.6);
            backdrop-filter: blur(4px);
            z-index: 1000;
            display: none;
            align-items: center;
            justify-content: center;
        }

        .modal {
            background-color: #0f172a;
            border: 1px solid var(--border-color);
            border-radius: 12px;
            width: 90%;
            max-width: 500px;
            box-shadow: 0 10px 40px rgba(0, 0, 0, 0.5);
            animation: modal-anim 0.3s cubic-bezier(0.16, 1, 0.3, 1);
        }

        @keyframes modal-anim {
            from { transform: scale(0.95); opacity: 0; }
            to { transform: scale(1); opacity: 1; }
        }

        .modal-header {
            padding: 1.25rem 1.5rem;
            border-bottom: 1px solid var(--border-color);
        }

        .modal-body {
            padding: 1.5rem;
        }

        .modal-footer {
            padding: 1rem 1.5rem;
            border-top: 1px solid var(--border-color);
            display: flex;
            justify-content: flex-end;
            gap: 0.75rem;
        }

        /* Toast notifications */
        .toast-container {
            position: fixed;
            bottom: 1.5rem;
            right: 1.5rem;
            z-index: 2000;
            display: flex;
            flex-direction: column;
            gap: 0.5rem;
        }

        .toast {
            background-color: var(--surface-color);
            border: 1px solid var(--border-color);
            color: var(--text-primary);
            padding: 0.75rem 1.25rem;
            border-radius: 8px;
            box-shadow: 0 10px 25px rgba(0, 0, 0, 0.3);
            display: flex;
            align-items: center;
            gap: 0.75rem;
            min-width: 250px;
            backdrop-filter: blur(8px);
            border-left: 4px solid var(--primary);
            animation: toast-in 0.3s cubic-bezier(0.16, 1, 0.3, 1);
        }

        @keyframes toast-in {
            from { transform: translateY(100%); opacity: 0; }
            to { transform: translateY(0); opacity: 1; }
        }

        /* SVG charts */
        .chart-svg {
            width: 100%;
            height: 180px;
        }

        .chart-bar {
            fill: var(--primary);
            rx: 3;
            transition: height 0.5s ease, y 0.5s ease;
        }

        /* Login Layout */
        .login-wrapper {
            display: flex;
            align-items: center;
            justify-content: center;
            width: 100vw;
            height: 100vh;
            position: fixed;
            top: 0;
            left: 0;
            background-color: var(--bg-color);
            z-index: 999;
        }

        .login-card {
            width: 100%;
            max-width: 400px;
            background-color: var(--surface-color);
            border: 1px solid var(--border-color);
            padding: 2.5rem;
            border-radius: 16px;
            backdrop-filter: blur(12px);
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
        }

        .hidden { display: none !important; }

        /* Responsive Mobile Layout adjustments */
        @media(max-width: 768px) {
            body { flex-direction: column; }
            aside {
                width: 100%;
                height: auto;
                position: relative;
                padding: 1rem 1.5rem;
                border-right: none;
                border-bottom: 1px solid var(--border-color);
            }
            .logo { margin-bottom: 1rem; }
            .nav-links { flex-direction: row; flex-wrap: wrap; gap: 0.25rem; }
            .nav-link { padding: 0.5rem 0.75rem; font-size: 0.8125rem; }
            .nav-link.active { border-left: none; border-bottom: 3px solid var(--primary); border-radius: 8px 8px 0 0; padding-left: 0.75rem; padding-bottom: calc(0.5rem - 3px); }
            main { margin-left: 0; width: 100%; padding: 1.5rem; }
        }
    </style>
</head>
<body>

    <!-- LOGIN PAGE VIEW -->
    <div id="login-page" class="login-wrapper hidden">
        <div class="login-card">
            <div class="logo text-center" style="justify-content: center;">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect>
                    <path d="M7 11V7a5 5 0 0 1 10 0v4"></path>
                </svg>
                <span>ESP-Attend Admin</span>
            </div>
            <p class="text-muted text-center" style="margin-top: -1.5rem; margin-bottom: 2rem;">Offline RFID Management System</p>
            <form id="login-form">
                <div class="form-group">
                    <label for="username">Username</label>
                    <input type="text" id="username" placeholder="Enter username" required>
                </div>
                <div class="form-group" style="margin-bottom: 1.5rem;">
                    <label for="password">Password</label>
                    <input type="password" id="password" placeholder="Enter password" required>
                </div>
                <button type="submit" class="btn btn-primary w-full" style="justify-content: center; padding: 0.75rem;">Login</button>
            </form>
        </div>
    </div>

    <!-- MAIN APP SCREEN -->
    <!-- SIDEBAR -->
    <aside id="sidebar-nav">
        <div class="logo">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                <path d="M22 19a2 2 0 0 1-2 2H4a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h5l2 3h9a2 2 0 0 1 2 2z"></path>
                <circle cx="12" cy="13" r="3"></circle>
            </svg>
            <span>ESP-Attend</span>
        </div>
        <ul class="nav-links">
            <li><a class="nav-link active" onclick="switchPage('dashboard')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="3" width="7" height="9"></rect><rect x="14" y="3" width="7" height="5"></rect><rect x="14" y="12" width="7" height="9"></rect><rect x="3" y="16" width="7" height="5"></rect></svg>
                Dashboard
            </a></li>
            <li><a class="nav-link" onclick="switchPage('students')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M17 21v-2a4 4 0 0 0-4-4H5a4 4 0 0 0-4 4v2"></path><circle cx="9" cy="7" r="4"></circle><path d="M23 21v-2a4 4 0 0 0-3-3.87"></path><path d="M16 3.13a4 4 0 0 1 0 7.75"></path></svg>
                Students
            </a></li>
            <li><a class="nav-link" onclick="switchPage('attendance')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"></path><polyline points="14 2 14 8 20 8"></polyline><line x1="16" y1="13" x2="8" y2="13"></line><line x1="16" y1="17" x2="8" y2="17"></line><polyline points="10 9 9 9 8 9"></polyline></svg>
                Attendance
            </a></li>
            <li><a class="nav-link" onclick="switchPage('reports')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><line x1="18" y1="20" x2="18" y2="10"></line><line x1="12" y1="20" x2="12" y2="4"></line><line x1="6" y1="20" x2="6" y2="14"></line></svg>
                Reports
            </a></li>
            <li><a class="nav-link" onclick="switchPage('settings')">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 1 1-2.83 2.83l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-4 0v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 1 1-2.83-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1 0-4h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 1 1 2.83-2.83l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 4 0v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 1 1 2.83 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 0 4h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>
                Settings
            </a></li>
            <li style="margin-top: auto;"><a class="nav-link" onclick="logout()" style="color: #f87171;">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4"></path><polyline points="16 17 21 12 16 7"></polyline><line x1="21" y1="12" x2="9" y2="12"></line></svg>
                Logout
            </a></li>
        </ul>
    </aside>

    <!-- CONTENT WRAPPER -->
    <main>
        <!-- HEADER -->
        <div class="flex justify-between align-center" style="border-bottom: 1px solid var(--border-color); padding-bottom: 1rem; margin-bottom: 2rem;">
            <div>
                <h1 id="page-title">Dashboard</h1>
                <p id="page-subtitle" class="text-muted">Welcome to the Offline Attendance System.</p>
            </div>
            <div class="flex gap-2 align-center">
                <span class="status-pill">
                    <span class="status-dot"></span>
                    <span id="ip-address-display">Connecting...</span>
                </span>
                <span id="esp-time-display" class="status-pill text-xs">--:--:--</span>
                <button class="btn btn-secondary btn-sm" onclick="syncTime()">Sync Time</button>
            </div>
        </div>

        <!-- 1. DASHBOARD VIEW -->
        <section id="dashboard-view" class="view-section">
            <!-- Active Session Panel (Top Banner) -->
            <div class="card" style="border-left: 4px solid var(--primary); margin-bottom: 1.5rem;">
                <div class="flex justify-between align-center" style="flex-wrap: wrap; gap: 1rem;">
                    <div class="flex align-center gap-2">
                        <div style="background: rgba(99, 102, 241, 0.15); padding: 0.6rem; border-radius: 8px; color: var(--primary);">
                            <svg width="22" height="22" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><path d="M12 2v4M12 18v4M4.93 4.93l2.83 2.83M16.24 16.24l2.83 2.83M2 12h4M18 12h4M4.93 19.07l2.83-2.83M16.24 7.76l2.83-2.83"></path></svg>
                        </div>
                        <div>
                            <div class="flex align-center gap-2">
                                <span id="session-status-badge" class="badge badge-success">Active</span>
                                <h3 id="active-session-label" style="font-size: 1.25rem; font-weight: 700; color: var(--text-primary);">General</h3>
                            </div>
                            <div class="flex gap-3 text-xs text-muted" style="margin-top: 0.25rem; flex-wrap: wrap;">
                                <span>Dept: <strong id="sess-meta-dept" class="text-primary">-</strong></span>
                                <span>Sem: <strong id="sess-meta-sem" class="text-primary">-</strong></span>
                                <span>Sec: <strong id="sess-meta-sec" class="text-primary">-</strong></span>
                                <span>Started: <strong id="sess-meta-time" class="text-primary">-</strong></span>
                                <span>Duration: <strong id="sess-meta-timer" class="text-primary">00:00:00</strong></span>
                                <span>Presents: <strong id="sess-meta-presents" class="badge badge-info">0</strong></span>
                            </div>
                        </div>
                    </div>
                    <div class="flex gap-2" style="align-items: center; flex-wrap: wrap;">
                        <button type="button" class="btn btn-primary" onclick="openNewSessionModal()">
                            <svg width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><line x1="12" y1="5" x2="12" y2="19"></line><line x1="5" y1="12" x2="19" y2="12"></line></svg>
                            New Session
                        </button>
                        <button type="button" id="btn-end-session" class="btn btn-secondary" onclick="endSession()" style="display: none;">
                            <svg width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><rect x="6" y="6" width="12" height="12" rx="2"></rect></svg>
                            End Session
                        </button>
                    </div>
                </div>
            </div>

            <div class="grid dashboard-grid">
                <div class="card">
                    <p class="card-title">Today's Attendance</p>
                    <p id="stat-today-count" class="card-value">-</p>
                    <p class="card-desc">Total presents scanned today</p>
                </div>
                <div class="card">
                    <p class="card-title">Total Students</p>
                    <p id="stat-total-students" class="card-value">-</p>
                    <p class="card-desc">Registered students in DB</p>
                </div>
                <div class="card">
                    <p class="card-title">SD Storage</p>
                    <p id="stat-sd-used" class="card-value">-</p>
                    <p id="stat-sd-free" class="card-desc">Checking SD details...</p>
                </div>
                <div class="card">
                    <p class="card-title">System Uptime</p>
                    <p id="stat-uptime" class="card-value">-</p>
                    <p id="stat-wifi-rssi" class="card-desc">Loading uptime...</p>
                </div>
            </div>

            <div class="grid" style="grid-template-columns: 2fr 1fr; gap: 1.5rem;">
                <!-- Live Scan Notifications -->
                <div class="card live-scan-card">
                    <h3>Live Scan Notification</h3>
                    <p class="text-muted">RFID reader activity log</p>
                    <div id="live-scan-box" class="live-notification">
                        <div id="live-scan-icon" class="live-scan-icon-container">
                            <svg width="24" height="24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><rect x="2" y="4" width="20" height="16" rx="2" ry="2"></rect><line x1="12" y1="4" x2="12" y2="20"></line></svg>
                        </div>
                        <div>
                            <h4 id="live-scan-name" style="font-size: 1.125rem;">No card scanned</h4>
                            <p id="live-scan-detail" class="text-muted" style="margin-top: 0.25rem;">Scan a registered student card or unknown card on the device.</p>
                            <div class="flex gap-2" style="margin-top: 0.5rem; font-size: 0.75rem;">
                                <span id="live-scan-uid" class="text-muted">UID: --</span>
                                <span id="live-scan-time" class="text-muted">Time: --</span>
                            </div>
                        </div>
                    </div>
                </div>

                <!-- System Stats / Summary Chart -->
                <div class="card">
                    <h3>Quick Chart</h3>
                    <p class="text-muted">Presents by department today</p>
                    <div style="margin-top: 1rem;">
                        <svg id="dept-chart" class="chart-svg" viewBox="0 0 300 180">
                            <!-- JS will populate bars here -->
                        </svg>
                    </div>
                </div>
            </div>

            <!-- Scheduled Sessions & Session History Grid -->
            <div class="grid" style="grid-template-columns: 1fr 1fr; gap: 1.5rem; margin-top: 1.5rem;">
                <!-- Today's Scheduled Sessions -->
                <div class="card">
                    <h3>Today's Scheduled Sessions</h3>
                    <p class="text-muted text-xs">Calendar scheduled classes for today</p>
                    <div id="scheduled-sessions-list" style="margin-top: 1rem; max-height: 220px; overflow-y: auto;">
                        <p class="text-muted text-xs text-center">No scheduled sessions for today.</p>
                    </div>
                </div>

                <!-- Session History -->
                <div class="card">
                    <div class="flex justify-between align-center">
                        <h3>Session History</h3>
                        <button class="btn btn-secondary btn-sm" onclick="clearSessionHistory()" style="color: var(--danger);">Clear History</button>
                    </div>
                    <p class="text-muted text-xs">Recently ended class sessions</p>
                    <div id="session-history-list" style="margin-top: 1rem; max-height: 220px; overflow-y: auto;">
                        <p class="text-muted text-xs text-center">No session history available.</p>
                    </div>
                </div>
            </div>

            <!-- Future Scheduled Sessions -->
            <div class="card" style="margin-top: 1.5rem;">
                <h3>Future Sessions</h3>
                <p class="text-muted text-xs">All dated scheduled classes outside today's schedule</p>
                <div id="future-sessions-list" style="margin-top: 1rem; max-height: 220px; overflow-y: auto;">
                    <p class="text-muted text-xs text-center">No future sessions scheduled.</p>
                </div>
            </div>
        </section>

        <!-- 2. STUDENTS VIEW -->
        <section id="students-view" class="view-section hidden">
            <div class="flex justify-between align-center" style="margin-bottom: 1.5rem;">
                <div class="flex gap-2">
                    <input type="text" id="student-search-input" placeholder="Search by name, roll, UID..." oninput="fetchStudents()">
                </div>
                <button class="btn btn-primary" onclick="openStudentModal()">
                    <svg width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><line x1="12" y1="5" x2="12" y2="19"></line><line x1="5" y1="12" x2="19" y2="12"></line></svg>
                    Add Student
                </button>
            </div>

            <div class="table-container">
                <table>
                    <thead>
                        <tr>
                            <th>RFID UID</th>
                            <th>Name</th>
                            <th>Roll Number</th>
                            <th>Department</th>
                            <th>Semester</th>
                            <th>Session</th>
                            <th>Contact</th>
                            <th>Actions</th>
                        </tr>
                    </thead>
                    <tbody id="students-table-body">
                        <!-- JS populated -->
                    </tbody>
                </table>
            </div>
        </section>

        <!-- 3. ATTENDANCE VIEW -->
        <section id="attendance-view" class="view-section hidden">
            <div class="filter-bar">
                <input type="text" id="att-search" placeholder="Search name/roll/UID..." oninput="fetchAttendance()">
                <input type="date" id="att-date-filter" onchange="fetchAttendance()">
                <input type="text" id="att-subject-filter" placeholder="Filter by subject..." oninput="fetchAttendance()">
                <select id="att-dept-filter" onchange="fetchAttendance()">
                    <option value="">All Departments</option>
                    <option value="CSE">CSE</option>
                    <option value="EEE">EEE</option>
                    <option value="CE">Civil Eng.</option>
                    <option value="ME">Mech Eng.</option>
                    <option value="BBA">BBA</option>
                    <option value="English">English</option>
                </select>
                <select id="att-sem-filter" onchange="fetchAttendance()">
                    <option value="">All Semesters</option>
                    <option value="1st">1st Semester</option>
                    <option value="2nd">2nd Semester</option>
                    <option value="3rd">3rd Semester</option>
                    <option value="4th">4th Semester</option>
                    <option value="5th">5th Semester</option>
                    <option value="6th">6th Semester</option>
                    <option value="7th">7th Semester</option>
                    <option value="8th">8th Semester</option>
                </select>
                <button type="button" class="btn btn-secondary btn-sm" onclick="clearAttFilters()" style="margin-left: auto;">Reset</button>
            </div>

            <div class="table-container">
                <table>
                    <thead>
                        <tr>
                            <th>Date</th>
                            <th>Time</th>
                            <th>Subject</th>
                            <th>UID</th>
                            <th>Name</th>
                            <th>Roll</th>
                            <th>Dept</th>
                            <th>Semester</th>
                            <th>Status</th>
                        </tr>
                    </thead>
                    <tbody id="attendance-table-body">
                        <!-- JS populated -->
                    </tbody>
                </table>
            </div>
        </section>

        <!-- 4. REPORTS VIEW -->
        <section id="reports-view" class="view-section hidden">
            <div class="grid gap-3" style="grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); margin-bottom: 2rem;">
                <div class="card">
                    <h3>Daily Report</h3>
                    <p class="text-muted">Presents breakdown per department for a specific day.</p>
                    <div class="flex gap-2" style="margin-top: 1.5rem;">
                        <input type="date" id="report-daily-date" style="flex-grow: 1;">
                        <button class="btn btn-primary" onclick="generateDailyReport()">Generate</button>
                        <button id="download-daily-report" class="btn btn-secondary" onclick="downloadDailyReport()" style="display: none;">Download CSV</button>
                    </div>
                    <div id="daily-report-output" style="margin-top: 1.5rem;"></div>
                </div>

                <div class="card">
                    <h3>Monthly Report</h3>
                    <p class="text-muted">Total working/present days per student for a given month.</p>
                    <div class="flex gap-2" style="margin-top: 1.5rem;">
                        <input type="month" id="report-monthly-month" style="flex-grow: 1;">
                        <button class="btn btn-primary" onclick="generateMonthlyReport()">Generate</button>
                        <button id="download-monthly-report" class="btn btn-secondary" onclick="downloadMonthlyReport()" style="display: none;">Download CSV</button>
                    </div>
                    <div id="monthly-report-output" style="margin-top: 1.5rem;"></div>
                </div>
            </div>

            <div class="card">
                <h3>Raw Database Download & Control</h3>
                <p class="text-muted">Download attendance logs directly or manage backups on the SD card.</p>
                <div class="flex gap-2" style="margin-top: 1.5rem;">
                    <!-- Download is performed by JavaScript so it can send Authorization headers. -->
                    <a id="csv-download-link" href="#" onclick="downloadCsv(event)" class="btn btn-primary">
                        <svg width="18" height="18" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4"></path><polyline points="7 10 12 15 17 10"></polyline><line x1="12" y1="15" x2="12" y2="3"></line></svg>
                        Download attendance.csv
                    </a>
                    <button class="btn btn-secondary" onclick="triggerBackup()">Backup Database</button>
                </div>
            </div>
        </section>

        <!-- 5. SETTINGS VIEW -->
        <section id="settings-view" class="view-section hidden">
            <div class="grid gap-3" style="grid-template-columns: repeat(auto-fit, minmax(400px, 1fr));">
                <!-- System Configuration -->
                <div class="card">
                    <h3>System Parameters</h3>
                    <p class="text-muted" style="margin-bottom: 1.5rem;">Device & operation variables.</p>
                    <form id="settings-form">
                        <div class="form-group">
                            <label for="set-device-name">Device Name</label>
                            <input type="text" id="set-device-name" required>
                        </div>

                        <div class="form-group">
                            <label for="set-admin-user">Admin Username</label>
                            <input type="text" id="set-admin-user" required>
                        </div>
                        <div class="form-group" style="margin-bottom: 1.5rem;">
                            <label for="set-admin-pass">Admin Password (leave blank to keep current)</label>
                            <input type="password" id="set-admin-pass" placeholder="••••••••">
                        </div>
                        <button type="submit" class="btn btn-primary">Save System Parameters</button>
                    </form>
                </div>

                <!-- Wi-Fi Settings -->
                <div class="card">
                    <h3>Wi-Fi & Network</h3>
                    <p class="text-muted" style="margin-bottom: 1.5rem;">SSID configuration. ESP32 will fall back to AP if connection fails.</p>
                    <form id="wifi-form">
                        <div class="form-group">
                            <label for="set-wifi-ssid">Network SSID</label>
                            <input type="text" id="set-wifi-ssid" placeholder="Enter Wi-Fi network name">
                        </div>
                        <div class="form-group" style="margin-bottom: 1.5rem;">
                            <label for="set-wifi-pass">Wi-Fi Password</label>
                            <input type="password" id="set-wifi-pass" placeholder="Enter Wi-Fi password">
                        </div>
                        <button type="submit" class="btn btn-primary">Save Wi-Fi Settings</button>
                    </form>

                    <div style="margin-top: 2rem; border-top: 1px solid var(--border-color); padding-top: 1.5rem;">
                        <h4 style="color: #f87171; margin-bottom: 0.5rem;">Danger Zone</h4>
                        <p class="text-muted" style="margin-bottom: 1rem;">Restart the ESP32 to apply Wi-Fi changes immediately.</p>
                        <button class="btn btn-danger" onclick="restartDevice()">Restart ESP32 Device</button>
                    </div>
                </div>
            </div>
        </section>
    </main>

    <!-- STUDENT EDIT/ADD MODAL -->
    <div id="student-modal" class="modal-overlay" onclick="if(event.target===this) closeStudentModal()">
        <div class="modal">
            <div class="modal-header">
                <h3 id="modal-title">Register Student</h3>
            </div>
            <form id="student-form" novalidate>
                <div class="modal-body">
                    <div class="form-group">
                        <label for="stud-uid">RFID Card UID</label>
                        <div class="flex gap-2">
                            <input type="text" id="stud-uid" placeholder="Card UID (e.g. A1B2C3D4)" required style="flex-grow: 1;">
                            <button type="button" class="btn btn-secondary btn-sm" id="btn-scan-uid" onclick="enableScanCapture()">
                                Scan Card
                            </button>
                        </div>
                        <span id="scan-capture-status" class="text-xs text-muted" style="margin-top: 0.25rem;"></span>
                    </div>
                    <div class="form-group">
                        <label for="stud-name">Full Name</label>
                        <input type="text" id="stud-name" placeholder="Hasan Ahmed" required>
                    </div>
                    <div class="form-group">
                        <label for="stud-roll">Roll Number</label>
                        <input type="text" id="stud-roll" placeholder="1001" required>
                    </div>
                    <div class="form-row">
                        <div class="form-group">
                            <label for="stud-dept">Department</label>
                            <input type="text" id="stud-dept" placeholder="CSE">
                        </div>
                        <div class="form-group">
                            <label for="stud-sem">Semester</label>
                            <input type="text" id="stud-sem" placeholder="5th">
                        </div>
                    </div>
                    <div class="form-group">
                        <label for="stud-session">Session</label>
                        <input type="text" id="stud-session" placeholder="2022-2023">
                    </div>
                    <div class="form-row">
                        <div class="form-group">
                            <label for="stud-phone">Phone (Optional)</label>
                            <input type="text" id="stud-phone" placeholder="01700-000000">
                        </div>
                        <div class="form-group">
                            <label for="stud-email">Email (Optional)</label>
                            <input type="text" id="stud-email" placeholder="someone@gmail.com">
                        </div>
                    </div>
                </div>
                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" onclick="closeStudentModal()">Cancel</button>
                    <button type="submit" class="btn btn-primary" id="btn-save-student">Save Student</button>
                </div>
            </form>
        </div>
    </div>

    <!-- NEW SESSION MODAL -->
    <div id="session-modal" class="modal-overlay" onclick="if(event.target===this) closeSessionModal()">
        <div class="modal" style="max-width: 550px;">
            <div class="modal-header flex justify-between align-center">
                <h3 id="session-modal-title">Create New Session</h3>
                <button type="button" class="btn btn-secondary btn-sm" onclick="closeSessionModal()" style="padding: 0.2rem 0.5rem;">&times;</button>
            </div>
            
            <form id="session-form" onsubmit="submitSessionForm(event)">
                <div class="modal-body">
                    <div class="flex gap-2" style="margin-bottom: 1rem; border-bottom: 1px solid var(--border-color); padding-bottom: 0.75rem;">
                        <button type="button" id="tab-quick-sess" class="btn btn-sm btn-primary" onclick="setSessionType('quick')">Quick Session</button>
                        <button type="button" id="tab-sched-sess" class="btn btn-sm btn-secondary" onclick="setSessionType('schedule')">Schedule for Date/Time</button>
                    </div>

                    <div class="form-group">
                        <label for="sess-subject">Subject / Class Name *</label>
                        <input type="text" id="sess-subject" placeholder="e.g. Computer Networks, Physics Lab" required>
                    </div>
                    <div class="form-row">
                        <div class="form-group">
                            <label for="sess-teacher">Teacher Name (Optional)</label>
                            <input type="text" id="sess-teacher" placeholder="e.g. Dr. Alan Smith">
                        </div>
                        <div class="form-group">
                            <label for="sess-dept">Department</label>
                            <select id="sess-dept">
                                <option value="">Select Dept</option>
                                <option value="CSE">CSE</option>
                                <option value="EEE">EEE</option>
                                <option value="CE">Civil Eng.</option>
                                <option value="ME">Mech Eng.</option>
                                <option value="BBA">BBA</option>
                                <option value="English">English</option>
                            </select>
                        </div>
                    </div>
                    <div class="grid" style="grid-template-columns: 1fr 1fr 1fr; gap: 0.75rem;">
                        <div class="form-group">
                            <label for="sess-sem">Semester</label>
                            <select id="sess-sem">
                                <option value="">Select Sem</option>
                                <option value="1st">1st</option>
                                <option value="2nd">2nd</option>
                                <option value="3rd">3rd</option>
                                <option value="4th">4th</option>
                                <option value="5th">5th</option>
                                <option value="6th">6th</option>
                                <option value="7th">7th</option>
                                <option value="8th">8th</option>
                            </select>
                        </div>
                        <div class="form-group">
                            <label for="sess-section">Section</label>
                            <input type="text" id="sess-section" placeholder="e.g. A / B">
                        </div>
                        <div class="form-group">
                            <label for="sess-room">Room No. (Optional)</label>
                            <input type="text" id="sess-room" placeholder="e.g. Lab 3">
                        </div>
                    </div>

                    <div id="schedule-fields" style="display: none; background: rgba(255,255,255,0.03); padding: 0.75rem; border-radius: 6px; margin-bottom: 1rem; border: 1px dashed var(--border-color);">
                        <p class="text-xs text-muted" style="margin-bottom: 0.5rem; font-weight: 600;">Calendar Schedule Settings</p>
                        <div class="grid" style="grid-template-columns: 1fr 1fr 1fr; gap: 0.5rem;">
                            <div>
                                <label class="text-xs">Date</label>
                                <input type="date" id="sess-sched-date">
                            </div>
                            <div>
                                <label class="text-xs">Start Time</label>
                                <input type="time" id="sess-sched-start">
                            </div>
                            <div>
                                <label class="text-xs">End Time</label>
                                <input type="time" id="sess-sched-end">
                            </div>
                        </div>
                    </div>

                    <div class="form-group">
                        <label for="sess-notes">Notes / Objective (Optional)</label>
                        <input type="text" id="sess-notes" placeholder="Optional notes for this session">
                    </div>
                </div>

                <div class="modal-footer">
                    <button type="button" class="btn btn-secondary" onclick="closeSessionModal()">Cancel</button>
                    <button type="submit" id="btn-submit-session" class="btn btn-primary">Start Session</button>
                </div>
            </form>
        </div>
    </div>

    <!-- SESSION END SUMMARY MODAL -->
    <div id="session-summary-modal" class="modal-overlay" onclick="if(event.target===this) closeSessionSummaryModal()">
        <div class="modal" style="max-width: 450px;">
            <div class="modal-body text-center" style="text-align: center; padding: 2rem 1.5rem;">
                <div style="background: rgba(52, 211, 153, 0.15); width: 56px; height: 56px; border-radius: 50%; display: flex; align-items: center; justify-content: center; margin: 0 auto 1rem auto; color: #34d399;">
                    <svg width="32" height="32" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><polyline points="20 6 9 17 4 12"></polyline></svg>
                </div>
                <h3>Session Completed</h3>
                <p id="sum-subject" style="font-size: 1.25rem; font-weight: 700; color: var(--primary); margin: 0.25rem 0 1rem 0;">-</p>
                
                <div style="background: rgba(255,255,255,0.03); padding: 1rem; border-radius: 8px; margin-bottom: 1.5rem; text-align: left;" class="text-xs">
                    <div class="flex justify-between" style="margin-bottom: 0.5rem;">
                        <span class="text-muted">Start Time:</span>
                        <strong id="sum-start-time" class="text-primary">-</strong>
                    </div>
                    <div class="flex justify-between" style="margin-bottom: 0.5rem;">
                        <span class="text-muted">End Time:</span>
                        <strong id="sum-end-time" class="text-primary">-</strong>
                    </div>
                    <div class="flex justify-between" style="margin-bottom: 0.5rem;">
                        <span class="text-muted">Total Duration:</span>
                        <strong id="sum-duration" class="text-primary">-</strong>
                    </div>
                    <div class="flex justify-between">
                        <span class="text-muted">Present Students Scanned:</span>
                        <span id="sum-presents" class="badge badge-success">0</span>
                    </div>
                </div>

                <button type="button" class="btn btn-primary w-full" style="justify-content: center; padding: 0.75rem;" onclick="closeSessionSummaryModal()">Done</button>
            </div>
        </div>
    </div>

    <!-- TOAST NOTIFICATION CONTAINER -->
    <div id="toast-container" class="toast-container"></div>

    <script>
        // App State
        let currentView = 'dashboard';
        let authToken = localStorage.getItem('authToken') || '';
        let scanCaptureActive = false;
        let lastScanEpochSeen = 0;

        // API Helper
        async function apiRequest(endpoint, method = 'GET', data = null) {
            const headers = {
                'Content-Type': 'application/json'
            };
            if (authToken) {
                headers['Authorization'] = 'Bearer ' + authToken;
            }

            const options = { method, headers };
            if (data) {
                options.body = JSON.stringify(data);
            }

            // Keep credentials in the Authorization header; never put the token in URLs.
            const url = endpoint;

            try {
                const response = await fetch(url, options);
                if (response.status === 401) {
                    // Token expired or invalid — clear stored token and show login form
                    // without abruptly destroying the page state.
                    authToken = '';
                    localStorage.removeItem('authToken');
                    showToast('⚠️ Session expired. Please log in again.', 'danger');
                    // Give the toast time to show before switching to login view
                    setTimeout(() => {
                        document.getElementById('login-page').classList.remove('hidden');
                        document.getElementById('sidebar-nav').classList.add('hidden');
                        document.querySelector('main').classList.add('hidden');
                    }, 800);
                    throw new Error('Session expired. Please log in again.');
                }
                if (!response.ok) {
                    const errText = await response.text();
                    let errMsg = errText || 'API Error';
                    try {
                        const errJson = JSON.parse(errText);
                        if (errJson && errJson.error) errMsg = errJson.error;
                    } catch (e) {}
                    throw new Error(errMsg);
                }
                return await response.json();
            } catch (err) {
                console.error('API Request failed:', err);
                throw err;
            }
        }

        // HTML Sanitizer Helper
        function escapeHtml(str) {
            if (str === null || str === undefined) return '';
            return String(str)
                .replace(/&/g, '&amp;')
                .replace(/</g, '&lt;')
                .replace(/>/g, '&gt;')
                .replace(/"/g, '&quot;')
                .replace(/'/g, '&#039;');
        }

        // Authentication Guard
        function checkAuth() {
            if (!authToken) {
                document.getElementById('login-page').classList.remove('hidden');
                document.getElementById('sidebar-nav').classList.add('hidden');
                document.querySelector('main').classList.add('hidden');
            } else {
                document.getElementById('login-page').classList.add('hidden');
                document.getElementById('sidebar-nav').classList.remove('hidden');
                document.querySelector('main').classList.remove('hidden');
                initApp();
            }
        }

        // Login Submit
        document.getElementById('login-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            const adminUser = document.getElementById('username').value;
            const adminPass = document.getElementById('password').value;

            try {
                const res = await fetch('/api/login', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ username: adminUser, password: adminPass })
                });

                if (res.ok) {
                    const data = await res.json();
                    authToken = data.token;
                    localStorage.setItem('authToken', authToken);
                    showToast('Logged in successfully', 'success');
                    checkAuth();
                } else {
                    showToast('Invalid administrator credentials', 'danger');
                }
            } catch (err) {
                showToast('Connection to server failed', 'danger');
            }
        });

        function logout() {
            authToken = '';
            localStorage.removeItem('authToken');
            checkAuth();
        }

        // Page Switching
        function switchPage(pageId) {
            currentView = pageId;
            document.querySelectorAll('.view-section').forEach(sec => sec.classList.add('hidden'));
            document.querySelectorAll('.nav-link').forEach(link => link.classList.remove('active'));

            document.getElementById(pageId + '-view').classList.remove('hidden');
            
            // Highlight nav link
            const links = document.querySelectorAll('.nav-link');
            links.forEach(link => {
                const clickAttr = link.getAttribute('onclick');
                if (clickAttr && clickAttr.includes(pageId)) {
                    link.classList.add('active');
                }
            });

            // Update Header Title
            const titles = {
                dashboard: ['Dashboard', 'Overview and live RFID reader statuses.'],
                students: ['Students Management', 'Add, edit, delete, and match RFID credentials.'],
                attendance: ['Attendance Log', 'Search and review attendance records directly from SD Card.'],
                reports: ['Reports Center', 'Generate daily/monthly present registers and download data.'],
                settings: ['System Settings', 'Change operational settings, Wi-Fi configuration, and credentials.']
            };
            document.getElementById('page-title').innerText = titles[pageId][0];
            document.getElementById('page-subtitle').innerText = titles[pageId][1];

            // Trigger fetch for selected page
            if (pageId === 'dashboard') fetchDashboardData();
            if (pageId === 'students') fetchStudents();
            if (pageId === 'attendance') fetchAttendance();
            if (pageId === 'reports') {
                const downloadBtn = document.querySelector('a[href^="/attendance.csv"]');
                if (downloadBtn) {
                    downloadBtn.setAttribute('href', '#');
                }
            }
            if (pageId === 'settings') loadSettings();
        }

        // Toast Messages
        function showToast(message, type = 'primary') {
            const container = document.getElementById('toast-container');
            const toast = document.createElement('div');
            toast.className = `toast`;
            toast.style.borderLeftColor = `var(--${type})`;
            
            // Icon
            const iconSvg = {
                success: '<svg width="18" height="18" fill="none" stroke="#34d399" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><polyline points="20 6 9 17 4 12"></polyline></svg>',
                danger: '<svg width="18" height="18" fill="none" stroke="#f87171" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><circle cx="12" cy="12" r="10"></circle><line x1="15" y1="9" x2="9" y2="15"></line><line x1="9" y1="9" x2="15" y2="15"></line></svg>',
                warning: '<svg width="18" height="18" fill="none" stroke="#fbbf24" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"></path><line x1="12" y1="9" x2="12" y2="13"></line><line x1="12" y1="17" x2="12.01" y2="17"></line></svg>',
                primary: '<svg width="18" height="18" fill="none" stroke="#a78bfa" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><circle cx="12" cy="12" r="10"></circle><line x1="12" y1="16" x2="12" y2="12"></line><line x1="12" y1="8" x2="12.01" y2="8"></line></svg>'
            }[type] || '';

            toast.innerHTML = `${iconSvg}<span>${message}</span>`;
            container.appendChild(toast);

            setTimeout(() => {
                toast.style.animation = 'toast-in 0.3s reverse forwards';
                toast.addEventListener('animationend', () => toast.remove());
            }, 3000);
        }

        // Dashboard Data Loading
        async function fetchDashboardData() {
            try {
                const data = await apiRequest('/api/dashboard');
                
                document.getElementById('stat-today-count').innerText = data.todayAttendance;
                document.getElementById('stat-total-students').innerText = data.studentsCount;
                
                // SD Storage Details
                if (data.sdStatus.available) {
                    const totalMB = (data.sdStatus.totalBytes / (1024 * 1024)).toFixed(1);
                    const usedMB = (data.sdStatus.usedBytes / (1024 * 1024)).toFixed(2);
                    const freeMB = (totalMB - usedMB).toFixed(1);
                    document.getElementById('stat-sd-used').innerText = `${usedMB} MB`;
                    document.getElementById('stat-sd-free').innerText = `Used out of ${totalMB} MB (${freeMB} MB Free)`;
                } else {
                    document.getElementById('stat-sd-used').innerText = 'ERROR';
                    document.getElementById('stat-sd-free').innerText = 'SD card not inserted/accessible';
                }

                // Wi-Fi Display
                document.getElementById('stat-uptime').innerText = data.uptime;
                if (data.wifi.mode === 'AP') {
                    document.getElementById('stat-wifi-rssi').innerText = `Fallback Access Point: ${data.wifi.ssid}`;
                } else {
                    document.getElementById('stat-wifi-rssi').innerText = `Connected RSSI: ${data.wifi.rssi} dBm`;
                }
                
                document.getElementById('ip-address-display').innerText = data.wifi.ip;
                document.getElementById('esp-time-display').innerText = data.systemTime.split(' ')[1] || '--:--:--';

                // Real-time scan updates
                updateLiveScanDisplay(data.lastScan);

                // Render dynamic SVG chart from department counts
                fetchDailyStatsForChart(data.systemTime.split(' ')[0]);

            } catch (err) {
                console.error(err);
            }
        }

        function updateLiveScanDisplay(scan) {
            const nameEl = document.getElementById('live-scan-name');
            const detailEl = document.getElementById('live-scan-detail');
            const uidEl = document.getElementById('live-scan-uid');
            const timeEl = document.getElementById('live-scan-time');
            const boxEl = document.getElementById('live-scan-box');
            const iconEl = document.getElementById('live-scan-icon');

            if (!scan || scan.status === 'None') {
                return;
            }

            // Update live-fill in modal if scan Capture is active
            if (scanCaptureActive && scan.epoch > lastScanEpochSeen) {
                captureStudentUid(scan.uid);
            }
            lastScanEpochSeen = scan.epoch;

            nameEl.innerText = scan.name;
            uidEl.innerText = 'UID: ' + scan.uid;
            timeEl.innerText = 'Time: ' + scan.time;

            // Reset classes
            boxEl.className = 'live-notification';
            iconEl.className = 'live-scan-icon-container';

            if (scan.status === 'Success') {
                iconEl.classList.add('success');
                detailEl.innerText = `Attendance logged. Status: Present.`;
                boxEl.style.borderLeftColor = 'var(--success)';
            } else if (scan.status === 'Duplicate') {
                // Silently ignore duplicate cooldown scans - do not display message
                return;
            } else {
                iconEl.classList.add('danger');
                detailEl.innerText = `Unregistered card presented. Assign to a student.`;
                boxEl.style.borderLeftColor = 'var(--danger)';
            }
        }

        async function fetchDailyStatsForChart(date) {
            try {
                const stats = await apiRequest(`/api/reports/daily?date=${date}`);
                const chartSvg = document.getElementById('dept-chart');

                const depts = Object.keys(stats);
                if (depts.length === 0) {
                    chartSvg.innerHTML = `<text x="150" y="90" fill="var(--text-secondary)" text-anchor="middle">No records scanned today</text>`;
                    return;
                }

                const counts = Object.values(stats);
                const maxVal = Math.max(...counts) || 1;
                const chartHeight = 120;
                const barWidth = 35;
                const spacing = 15;
                const startX = 40;

                let svgContent = `<line x1="30" y1="10" x2="30" y2="140" stroke="var(--border-color)" stroke-width="1" />`;
                svgContent += `<line x1="30" y1="140" x2="280" y2="140" stroke="var(--border-color)" stroke-width="1" />`;

                depts.forEach((dept, index) => {
                    const count = stats[dept];
                    const barHeight = (count / maxVal) * chartHeight;
                    const x = startX + index * (barWidth + spacing);
                    const y = 140 - barHeight;

                    svgContent += `
                        <rect class="chart-bar" x="${x}" y="${y}" width="${barWidth}" height="${barHeight}" />
                        <text x="${x + barWidth/2}" y="${y - 5}" fill="var(--text-primary)" font-size="10" text-anchor="middle">${count}</text>
                        <text x="${x + barWidth/2}" y="155" fill="var(--text-secondary)" font-size="10" text-anchor="middle">${escapeHtml(dept)}</text>
                    `;
                });
                chartSvg.innerHTML = svgContent;
            } catch (err) {
                console.error(err);
            }
        }

        // Student Database View
        async function fetchStudents() {
            const query = document.getElementById('student-search-input').value;
            try {
                const students = await apiRequest(`/api/students?search=${encodeURIComponent(query)}`);
                const tbody = document.getElementById('students-table-body');
                tbody.innerHTML = '';

                if (students.length === 0) {
                    tbody.innerHTML = `<tr><td colspan="8" class="text-center text-muted">No students found.</td></tr>`;
                    return;
                }

                students.forEach(s => {
                    const tr = document.createElement('tr');
                    const safeUid = escapeHtml(s.uid);
                    const encUid = encodeURIComponent(s.uid);
                    tr.innerHTML = `
                        <td><code>${safeUid}</code></td>
                        <td><strong>${escapeHtml(s.name)}</strong></td>
                        <td>${escapeHtml(s.roll)}</td>
                        <td>${escapeHtml(s.dept)}</td>
                        <td>${escapeHtml(s.sem)}</td>
                        <td>${escapeHtml(s.session || s.sec || '-')}</td>
                        <td class="text-xs">${escapeHtml(s.phone || '-')}<br>${escapeHtml(s.email || '-')}</td>
                        <td>
                            <button type="button" class="btn btn-secondary btn-sm" onclick="editStudent('${encUid}')">Edit</button>
                            <button type="button" class="btn btn-danger btn-sm" onclick="deleteStudent('${encUid}', this)">Delete</button>
                        </td>
                    `;
                    tbody.appendChild(tr);
                });
            } catch (err) {
                showToast(err.message || 'Failed to load students', 'danger');
            }
        }

        // Card UID Scan Capture in Modal
        function enableScanCapture() {
            scanCaptureActive = true;
            document.getElementById('scan-capture-status').innerText = 'Please swipe/scan card on reader now...';
            document.getElementById('btn-scan-uid').classList.replace('btn-secondary', 'btn-primary');
            fetchDashboardData();
        }

        async function captureStudentUid(scannedUid) {
            disableScanCapture();
            const uid = (scannedUid || '').trim().toUpperCase();
            if (!uid) {
                showToast('No card UID was received', 'danger');
                return;
            }

            try {
                const students = await apiRequest('/api/students');
                const existing = students.find(student =>
                    (student.uid || '').trim().toUpperCase() === uid
                );

                if (existing) {
                    // During an edit, the currently assigned card is valid.
                    // Only a card belonging to a different student is a conflict.
                    const editingUid = document.getElementById('student-form').dataset.originalUid || '';
                    if (editingUid && (existing.uid || '').trim().toUpperCase() === editingUid.trim().toUpperCase()) {
                        document.getElementById('stud-uid').value = uid;
                        document.getElementById('scan-capture-status').innerText = 'Current card UID retained.';
                        showToast('Current card UID retained', 'success');
                        return;
                    }
                    document.getElementById('stud-uid').value = '';
                    document.getElementById('scan-capture-status').innerText = `This card is already assigned to ${existing.name || 'another student'}.`;
                    showToast(`Card already belongs to ${existing.name || 'another student'}`, 'warning');
                    return;
                }

                document.getElementById('stud-uid').value = uid;
                document.getElementById('scan-capture-status').innerText = 'New card UID captured.';
                showToast('New card UID captured successfully', 'success');
            } catch (err) {
                showToast(err.message || 'Could not check whether this card is registered', 'danger');
            }
        }

        function disableScanCapture() {
            scanCaptureActive = false;
            document.getElementById('scan-capture-status').innerText = '';
            document.getElementById('btn-scan-uid').classList.replace('btn-primary', 'btn-secondary');
        }

        // Modal Controls
        function openStudentModal() {
            document.getElementById('student-form').dataset.originalUid = '';
            document.getElementById('modal-title').innerText = 'Register Student';
            document.getElementById('student-form').reset();
            document.getElementById('stud-uid').readOnly = false;
            document.getElementById('student-modal').style.display = 'flex';
            disableScanCapture();
        }

        function closeStudentModal() {
            document.getElementById('student-modal').style.display = 'none';
            document.getElementById('student-form').dataset.originalUid = '';
            disableScanCapture();
        }

        async function editStudent(encodedUid) {
            const uid = decodeURIComponent(encodedUid);
            try {
                const students = await apiRequest('/api/students');
                const student = students.find(s => s.uid.trim().toUpperCase() === uid.trim().toUpperCase());
                if (!student) {
                    showToast('Student record not found', 'warning');
                    return;
                }

                document.getElementById('student-form').dataset.originalUid = student.uid;

                document.getElementById('modal-title').innerText = 'Edit Student Details';
                document.getElementById('stud-uid').value = student.uid;
                document.getElementById('stud-uid').readOnly = true;
                document.getElementById('stud-name').value = student.name;
                document.getElementById('stud-roll').value = student.roll;
                document.getElementById('stud-dept').value = student.dept;
                document.getElementById('stud-sem').value = student.sem;
                document.getElementById('stud-session').value = student.session || student.sec || '';
                document.getElementById('stud-phone').value = student.phone || '';
                document.getElementById('stud-email').value = student.email || '';

                document.getElementById('student-modal').style.display = 'flex';
            } catch (err) {
                showToast(err.message || 'Failed to retrieve student data', 'danger');
            }
        }

        async function saveStudent(e) {
            if (e) e.preventDefault();

            // Keep the identity of the record on the form itself. This makes
            // an edit independent of any transient page-level state.
            const editingUid = document.getElementById('student-form').dataset.originalUid || '';
            const editingStudent = editingUid.trim().length > 0;
            const uid = document.getElementById('stud-uid').value.trim().toUpperCase();
            const name = document.getElementById('stud-name').value.trim();
            const roll = document.getElementById('stud-roll').value.trim();

            if (!uid || !name || !roll) {
                showToast('Please enter RFID UID, Full Name, and Roll Number', 'warning');
                return;
            }

            // Give an immediate, clear message before trying to register a
            // card that is already assigned to another student. The ESP32
            // also enforces this same rule in Students::add().
            if (!editingStudent) {
                try {
                    const students = await apiRequest('/api/students');
                    const existing = students.find(student =>
                        (student.uid || '').trim().toUpperCase() === uid
                    );
                    if (existing) {
                        showToast(`This card is already assigned to ${existing.name || 'another student'}`, 'warning');
                        return;
                    }
                } catch (err) {
                    showToast(err.message || 'Could not verify the card UID', 'danger');
                    return;
                }
            }

            const studentData = {
                uid: uid,
                name: name,
                roll: roll,
                dept: document.getElementById('stud-dept').value.trim(),
                sem: document.getElementById('stud-sem').value.trim(),
                session: document.getElementById('stud-session').value.trim(),
                phone: document.getElementById('stud-phone').value.trim(),
                email: document.getElementById('stud-email').value.trim()
            };

            try {
                if (editingStudent) {
                    await apiRequest('/api/students', 'POST', { ...studentData, originalUid: editingUid });
                    showToast('Student information updated successfully', 'success');
                } else {
                    await apiRequest('/api/students', 'POST', studentData);
                    showToast('New student registered successfully', 'success');
                }
                closeStudentModal();
                fetchStudents();
            } catch (err) {
                showToast(err.message || 'Operation failed', 'danger');
            }
        }

        document.getElementById('student-form').addEventListener('submit', saveStudent);

        async function deleteStudent(encodedUid, button) {
            let uid = '';
            try {
                uid = decodeURIComponent(encodedUid).trim();
            } catch (err) {
                showToast('Invalid student UID', 'danger');
                return;
            }
            if (!confirm(`Are you sure you want to delete student with UID: ${uid}?`)) return;
            try {
                if (button) {
                    button.disabled = true;
                    button.innerText = 'Deleting...';
                }
                await apiRequest('/api/student-delete', 'POST', { uid });
                showToast('Student record deleted successfully', 'success');
                fetchStudents();
            } catch (err) {
                showToast(err.message || 'Failed to delete student', 'danger');
                if (button) {
                    button.disabled = false;
                    button.innerText = 'Delete';
                }
            }
        }

        // Attendance Log View
        async function fetchAttendance() {
            const search  = document.getElementById('att-search').value.trim();
            const date    = document.getElementById('att-date-filter').value.trim();
            const dept    = document.getElementById('att-dept-filter').value.trim();
            const sem     = document.getElementById('att-sem-filter').value.trim();
            const subject = document.getElementById('att-subject-filter').value.trim();

            let url = `/api/attendance?limit=200`;
            if (search)  url += `&search=${encodeURIComponent(search)}`;
            if (date)    url += `&date=${encodeURIComponent(date)}`;
            if (dept)    url += `&dept=${encodeURIComponent(dept)}`;
            if (sem)     url += `&sem=${encodeURIComponent(sem)}`;
            if (subject) url += `&subject=${encodeURIComponent(subject)}`;

            try {
                const logs = await apiRequest(url);
                const tbody = document.getElementById('attendance-table-body');
                tbody.innerHTML = '';

                if (!Array.isArray(logs) || logs.length === 0) {
                    tbody.innerHTML = `<tr><td colspan="9" class="text-center text-muted">No attendance logs found matching filters.</td></tr>`;
                    return;
                }

                logs.forEach(l => {
                    const tr = document.createElement('tr');
                    const subjectLabel = escapeHtml(l.subject || 'General');
                    const statusClass = (l.status || '').toLowerCase() === 'present' ? 'badge-success' : 'badge-warning';
                    tr.innerHTML = `
                        <td><code>${escapeHtml(l.date)}</code></td>
                        <td>${escapeHtml(l.time)}</td>
                        <td><span style="background:rgba(99,102,241,0.15);color:#a5b4fc;padding:2px 7px;border-radius:4px;font-size:0.75rem;font-weight:600;">${subjectLabel}</span></td>
                        <td><code>${escapeHtml(l.uid)}</code></td>
                        <td><strong>${escapeHtml(l.name)}</strong></td>
                        <td>${escapeHtml(l.roll)}</td>
                        <td>${escapeHtml(l.dept)}</td>
                        <td>${escapeHtml(l.sem)}</td>
                        <td><span class="badge ${statusClass}">${escapeHtml(l.status || '-')}</span></td>
                    `;
                    tbody.appendChild(tr);
                });
            } catch (err) {
                showToast(err.message || 'Failed to load attendance logs', 'danger');
            }
        }

        function clearAttFilters() {
            const searchEl = document.getElementById('att-search');
            const dateEl   = document.getElementById('att-date-filter');
            const subjEl   = document.getElementById('att-subject-filter');
            const deptEl   = document.getElementById('att-dept-filter');
            const semEl    = document.getElementById('att-sem-filter');

            if (searchEl) searchEl.value = '';
            if (dateEl)   dateEl.value = '';
            if (subjEl)   subjEl.value = '';
            if (deptEl)   { deptEl.value = ''; deptEl.selectedIndex = 0; }
            if (semEl)    { semEl.value = ''; semEl.selectedIndex = 0; }

            showToast('Attendance filters reset', 'primary');
            fetchAttendance();
        }

        function downloadCsv(e) {
            if (e) e.preventDefault();
            if (!authToken) {
                showToast('Please log in first', 'warning');
                return;
            }
            fetch('/attendance.csv', {
                headers: { 'Authorization': 'Bearer ' + authToken }
            }).then(async response => {
                if (!response.ok) throw new Error('CSV download failed');
                const blob = await response.blob();
                const url = URL.createObjectURL(blob);
                const link = document.createElement('a');
                link.href = url;
                link.download = 'attendance.csv';
                document.body.appendChild(link);
                link.click();
                link.remove();
                URL.revokeObjectURL(url);
            }).catch(err => showToast(err.message, 'danger'));
        }

        // Session management
        let activeSessionStartEpoch = 0;
        let liveTimerInterval = null;
        let currentSessionType = 'quick';
        let editingSessionId = null;

        function openNewSessionModal() {
            editingSessionId = null;
            document.getElementById('session-form').reset();
            document.getElementById('session-modal-title').innerText = 'Create New Session';
            document.getElementById('tab-quick-sess').style.display = '';
            document.getElementById('tab-sched-sess').style.display = '';
            setSessionType('quick');
            document.getElementById('session-modal').style.display = 'flex';
        }

        function closeSessionModal() {
            editingSessionId = null;
            document.getElementById('tab-quick-sess').style.display = '';
            document.getElementById('tab-sched-sess').style.display = '';
            document.getElementById('session-modal').style.display = 'none';
        }

        function setSessionType(type) {
            currentSessionType = type;
            const btnQuick = document.getElementById('tab-quick-sess');
            const btnSched = document.getElementById('tab-sched-sess');
            const schedFields = document.getElementById('schedule-fields');
            const submitBtn = document.getElementById('btn-submit-session');

            if (type === 'quick') {
                btnQuick.className = 'btn btn-sm btn-primary';
                btnSched.className = 'btn btn-sm btn-secondary';
                schedFields.style.display = 'none';
                submitBtn.innerText = 'Start Session';
            } else {
                btnQuick.className = 'btn btn-sm btn-secondary';
                btnSched.className = 'btn btn-sm btn-primary';
                schedFields.style.display = 'block';
                submitBtn.innerText = 'Schedule Session';
                
                const dateInput = document.getElementById('sess-sched-date');
                if (!dateInput.value) {
                    const today = new Date().toISOString().split('T')[0];
                    dateInput.value = today;
                }
            }
        }

        async function submitSessionForm(e) {
            if (e) e.preventDefault();

            const subject = document.getElementById('sess-subject').value.trim();
            if (!subject) {
                showToast('Subject is required', 'warning');
                return;
            }

            const payload = {
                subject: subject,
                teacher: document.getElementById('sess-teacher').value.trim(),
                dept: document.getElementById('sess-dept').value,
                sem: document.getElementById('sess-sem').value,
                section: document.getElementById('sess-section').value.trim(),
                room: document.getElementById('sess-room').value.trim(),
                notes: document.getElementById('sess-notes').value.trim()
            };

            if (currentSessionType === 'schedule' || editingSessionId) {
                const schedDate = document.getElementById('sess-sched-date').value;
                const schedStart = document.getElementById('sess-sched-start').value;
                const schedEnd = document.getElementById('sess-sched-end').value;

                if (!schedDate || !schedStart) {
                    showToast('Scheduled date and start time are required', 'warning');
                    return;
                }
                if (schedEnd && schedEnd < schedStart) {
                    showToast('End time cannot be earlier than start time', 'warning');
                    return;
                }

                payload.scheduledDate = schedDate;
                payload.scheduledStartTime = schedStart;
                payload.scheduledEndTime = schedEnd;
            }

            try {
                const submitBtn = document.getElementById('btn-submit-session');
                submitBtn.disabled = true;
                // Edit mode: update existing scheduled session
                if (editingSessionId) {
                    payload.id = editingSessionId;
                    await apiRequest('/api/session/update', 'POST', payload);
                    closeSessionModal();
                    showToast('Scheduled session updated', 'success');
                    loadSessionsLists();
                    return;
                }

                // Create mode
                const res = await apiRequest('/api/session/start', 'POST', payload);
                closeSessionModal();

                if (res.sessionStatus === 'Active') {
                    showToast(`Active Session started: ${res.subject}`, 'success');
                } else if (res.sessionStatus === 'Scheduled') {
                    showToast(`Session scheduled for ${payload.scheduledDate}`, 'success');
                } else {
                    showToast('Session saved', 'success');
                }

                loadActiveSession();
                loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to save session', 'danger');
            } finally {
                const submitBtn = document.getElementById('btn-submit-session');
                if (submitBtn) submitBtn.disabled = false;
            }
        }

        async function loadActiveSession() {
            try {
                const data = await apiRequest('/api/session');
                
                const badgeEl = document.getElementById('session-status-badge');
                const labelEl = document.getElementById('active-session-label');
                const deptEl = document.getElementById('sess-meta-dept');
                const semEl = document.getElementById('sess-meta-sem');
                const secEl = document.getElementById('sess-meta-sec');
                const timeEl = document.getElementById('sess-meta-time');
                const timerEl = document.getElementById('sess-meta-timer');
                const presentsEl = document.getElementById('sess-meta-presents');
                const btnEnd = document.getElementById('btn-end-session');

                if (data && data.status === 'Active') {
                    badgeEl.innerText = 'Active';
                    badgeEl.className = 'badge badge-success';
                    labelEl.innerText = data.subject || 'General';
                    deptEl.innerText = data.dept || '-';
                    semEl.innerText = data.sem || '-';
                    secEl.innerText = data.section || '-';
                    timeEl.innerText = data.startTime || '-';
                    presentsEl.innerText = data.presentsCount || 0;
                    btnEnd.style.display = 'inline-flex';

                    activeSessionStartEpoch = data.startEpoch || 0;
                    startLiveTimer();
                } else {
                    badgeEl.innerText = 'Inactive';
                    badgeEl.className = 'badge badge-warning';
                    labelEl.innerText = 'No Active Session';
                    deptEl.innerText = '-';
                    semEl.innerText = '-';
                    secEl.innerText = '-';
                    timeEl.innerText = '-';
                    timerEl.innerText = '00:00:00';
                    presentsEl.innerText = '0';
                    btnEnd.style.display = 'none';

                    activeSessionStartEpoch = 0;
                    stopLiveTimer();
                }
            } catch (err) {
                console.error('loadActiveSession error:', err);
            }
        }

        function startLiveTimer() {
            stopLiveTimer();
            updateLiveTimerDisplay();
            liveTimerInterval = setInterval(updateLiveTimerDisplay, 1000);
        }

        function stopLiveTimer() {
            if (liveTimerInterval) {
                clearInterval(liveTimerInterval);
                liveTimerInterval = null;
            }
        }

        function updateLiveTimerDisplay() {
            if (activeSessionStartEpoch <= 0) return;
            const now = Math.floor(Date.now() / 1000);
            let diffSec = Math.max(0, now - activeSessionStartEpoch);
            
            const hrs = Math.floor(diffSec / 3600);
            diffSec %= 3600;
            const mins = Math.floor(diffSec / 60);
            const secs = diffSec % 60;

            const pad = (n) => String(n).padStart(2, '0');
            const str = `${pad(hrs)}:${pad(mins)}:${pad(secs)}`;
            const timerEl = document.getElementById('sess-meta-timer');
            if (timerEl) timerEl.innerText = str;
        }

        async function endSession() {
            if (!confirm('Are you sure you want to end the current active session?')) return;
            try {
                // Send an explicit JSON body so ESPAsyncWebServer completes the
                // request-body callback consistently across library versions.
                const ended = await apiRequest('/api/session/end', 'POST', {});
                stopLiveTimer();

                document.getElementById('sum-subject').innerText = ended.subject || 'General';
                document.getElementById('sum-start-time').innerText = ended.startTime || '-';
                document.getElementById('sum-end-time').innerText = ended.endTime || '-';
                document.getElementById('sum-duration').innerText = ended.duration || '0 mins';
                document.getElementById('sum-presents').innerText = ended.presentsCount || 0;

                document.getElementById('session-summary-modal').style.display = 'flex';

                loadActiveSession();
                loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to end session', 'danger');
            }
        }

        function closeSessionSummaryModal() {
            document.getElementById('session-summary-modal').style.display = 'none';
        }

        async function loadSessionsLists() {
            try {
                const sessions = await apiRequest('/api/sessions');
                const schedContainer = document.getElementById('scheduled-sessions-list');
                const histContainer = document.getElementById('session-history-list');
                const futureContainer = document.getElementById('future-sessions-list');

                if (!Array.isArray(sessions)) return;

                // Use the local calendar date; UTC conversion can show the
                // wrong day for Bangladesh (UTC+6).
                const localNow = new Date();
                const todayStr = `${localNow.getFullYear()}-${String(localNow.getMonth() + 1).padStart(2, '0')}-${String(localNow.getDate()).padStart(2, '0')}`;
                const scheduledToday = sessions.filter(s => s.status === 'Scheduled' && (s.scheduledDate === todayStr || !s.scheduledDate));
                // Keep this independent of the computer's calendar being
                // correct: every dated scheduled session not shown in Today's
                // Schedule appears here, including one that is overdue.
                const futureSessions = sessions
                    .filter(s => s.status === 'Scheduled' && s.scheduledDate && s.scheduledDate !== todayStr)
                    .sort((a, b) => (a.scheduledDate + a.scheduledStartTime).localeCompare(b.scheduledDate + b.scheduledStartTime));
                const historyList = sessions.filter(s => s.status === 'Ended');

                if (scheduledToday.length === 0) {
                    schedContainer.innerHTML = '<p class="text-muted text-xs text-center">No scheduled sessions for today.</p>';
                } else {
                    let html = '';
                    scheduledToday.forEach(s => {
                        html += `
                            <div class="flex justify-between align-center" style="padding: 0.6rem; border-bottom: 1px solid var(--border-color); font-size: 0.8125rem;">
                                <div style="flex: 1; min-width: 0;">
                                    <strong style="color: var(--text-primary);">${escapeHtml(s.subject)}</strong>
                                    <div class="text-xs text-muted">${escapeHtml(s.dept || '-')} | ${escapeHtml(s.sem || '-')} | ${escapeHtml(s.scheduledDate)} (${escapeHtml(s.scheduledStartTime)}-${escapeHtml(s.scheduledEndTime || 'End')})</div>
                                </div>
                                <div class="flex" style="align-items: center; gap: 0.3rem; flex-shrink: 0;">
                                <div class="flex" style="align-items: center; gap: 0.35rem; flex-shrink: 0;">
                                    <button class="btn btn-secondary btn-sm" onclick="editScheduledSession('${escapeHtml(s.id)}')">Edit</button>
                                    <button class="btn btn-secondary btn-sm" onclick="cancelScheduledSession('${escapeHtml(s.id)}')" style="color: var(--danger);">Cancel</button>
                                </div>
                                    <button class="btn btn-secondary btn-sm" onclick="deleteScheduledSession('${escapeHtml(s.id)}')" title="Delete" style="padding: 0.25rem 0.4rem; color: var(--danger);">
                                        <svg width="14" height="14" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 01-2 2H7a2 2 0 01-2-2V6m3 0V4a2 2 0 012-2h4a2 2 0 012 2v2"/></svg>
                                    </button>
                                    <button class="btn btn-primary btn-sm" onclick="startScheduledNow('${escapeHtml(s.id)}')">Start Now</button>
                                </div>
                            </div>
                        `;
                    });
                    schedContainer.innerHTML = html;
                }

                if (futureSessions.length === 0) {
                    futureContainer.innerHTML = '<p class="text-muted text-xs text-center">No future sessions scheduled.</p>';
                } else {
                    let html = '';
                    futureSessions.slice(0, 12).forEach(s => {
                        html += `
                            <div class="flex justify-between align-center" style="padding: 0.6rem; border-bottom: 1px solid var(--border-color); font-size: 0.8125rem;">
                                <div style="flex: 1; min-width: 0;">
                                    <strong style="color: var(--text-primary);">${escapeHtml(s.subject)}</strong>
                                    <div class="text-xs text-muted">${escapeHtml(s.scheduledDate)} | ${escapeHtml(s.scheduledStartTime)}-${escapeHtml(s.scheduledEndTime || 'End')} | ${escapeHtml(s.dept || '-')} ${escapeHtml(s.sem || '')}</div>
                                </div>
                                <div class="flex" style="align-items: center; gap: 0.35rem; flex-shrink: 0;">
                                    <button class="btn btn-secondary btn-sm" onclick="editScheduledSession('${escapeHtml(s.id)}')">Edit</button>
                                    <button class="btn btn-secondary btn-sm" onclick="cancelScheduledSession('${escapeHtml(s.id)}')" style="color: var(--danger);">Cancel</button>
                                </div>
                            </div>
                        `;
                    });
                    futureContainer.innerHTML = html;
                }

                if (historyList.length === 0) {
                    histContainer.innerHTML = '<p class="text-muted text-xs text-center">No session history available.</p>';
                } else {
                    let html = '';
                    historyList.slice().reverse().slice(0, 10).forEach(s => {
                        html += `
                            <div class="flex justify-between align-center" style="padding: 0.6rem; border-bottom: 1px solid var(--border-color); font-size: 0.8125rem;">
                                <div>
                                    <strong style="color: var(--text-primary);">${escapeHtml(s.subject)}</strong>
                                    <div class="text-xs text-muted">${escapeHtml(s.date)} | ${escapeHtml(s.startTime)} - ${escapeHtml(s.endTime || '-')}</div>
                                </div>
                                <div class="flex" style="align-items: center; gap: 0.4rem;">
                                    <div class="text-right">
                                        <span class="badge badge-info">${escapeHtml(s.duration || '-')}</span>
                                        <div class="text-xs text-muted" style="margin-top: 2px;">${s.presentsCount || 0} presents</div>
                                    </div>
                                    <button class="btn btn-secondary btn-sm" onclick="deleteScheduledSession('${escapeHtml(s.id)}')" title="Delete from history" style="padding: 0.25rem 0.4rem; color: var(--danger);">
                                        <svg width="14" height="14" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" viewBox="0 0 24 24"><polyline points="3 6 5 6 21 6"/><path d="M19 6v14a2 2 0 01-2 2H7a2 2 0 01-2-2V6m3 0V4a2 2 0 012-2h4a2 2 0 012 2v2"/></svg>
                                    </button>
                                </div>
                            </div>
                        `;
                    });
                    histContainer.innerHTML = html;
                }
            } catch (err) {
                console.error('loadSessionsLists error:', err);
            }
        }

        async function startScheduledNow(sessionId) {
            try {
                const res = await apiRequest('/api/session/start', 'POST', { id: sessionId });
                showToast(`Session started: ${res.subject || 'Scheduled Session'}`, 'success');
                loadActiveSession();
                loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to start scheduled session', 'danger');
            }
        }

        async function editScheduledSession(sessionId) {
            try {
                const sessions = await apiRequest('/api/sessions');
                const target = sessions.find(s => s.id === sessionId);
                if (!target) { showToast('Session not found', 'danger'); return; }

                editingSessionId = sessionId;
                document.getElementById('session-modal-title').innerText = 'Edit Scheduled Session';

                document.getElementById('sess-subject').value = target.subject || '';
                document.getElementById('sess-teacher').value = target.teacher || '';
                document.getElementById('sess-dept').value = target.dept || '';
                document.getElementById('sess-sem').value = target.sem || '';
                document.getElementById('sess-section').value = target.section || '';
                document.getElementById('sess-room').value = target.room || '';
                document.getElementById('sess-notes').value = target.notes || '';

                // Force schedule tab and populate date/time fields
                setSessionType('schedule');
                document.getElementById('sess-sched-date').value = target.scheduledDate || '';
                document.getElementById('sess-sched-start').value = target.scheduledStartTime || '';
                document.getElementById('sess-sched-end').value = target.scheduledEndTime || '';

                // Hide Quick/Schedule tabs when editing — only schedule makes sense
                document.getElementById('tab-quick-sess').style.display = 'none';
                document.getElementById('tab-sched-sess').style.display = 'none';
                document.getElementById('btn-submit-session').innerText = 'Update Session';

                document.getElementById('session-modal').style.display = 'flex';
            } catch (err) {
                showToast('Failed to load session for editing', 'danger');
            }
        }

        async function deleteScheduledSession(sessionId) {
            if (!confirm('Are you sure you want to delete this session?')) return;
            try {
                await apiRequest('/api/session/delete', 'POST', { id: sessionId });
                showToast('Session deleted', 'success');
                loadActiveSession();
                loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to delete session', 'danger');
            }
        }

        async function cancelScheduledSession(sessionId) {
            if (!confirm('Cancel this scheduled class? This cannot be undone.')) return;
            try {
                await apiRequest('/api/session/delete', 'POST', { id: sessionId });
                showToast('Scheduled class cancelled', 'success');
                loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to cancel scheduled class', 'danger');
            }
        }

        async function cancelScheduledSession(sessionId) {
            if (!confirm('Cancel this scheduled class? This cannot be undone.')) return;
            try {
                await apiRequest('/api/session/delete', 'POST', { id: sessionId });
                showToast('Scheduled class cancelled', 'success');
                loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to cancel scheduled class', 'danger');
            }
        }

        async function clearSessionHistory() {
            if (!confirm('Clear all ended session history? Active and scheduled sessions will be kept.')) return;
            try {
                await apiRequest('/api/session/clear-history', 'POST');
                showToast('Session history cleared', 'success');
                await loadSessionsLists();
            } catch (err) {
                showToast(err.message || 'Failed to clear session history', 'danger');
            }
        }

        // Reports View
        let latestDailyReport = null;
        let latestMonthlyReport = null;

        function csvCell(value) {
            return `"${String(value ?? '').replace(/"/g, '""')}"`;
        }

        function downloadCsvFile(filename, headers, rows) {
            const csv = [headers, ...rows]
                .map(row => row.map(csvCell).join(','))
                .join('\r\n');
            const blob = new Blob([csv], { type: 'text/csv;charset=utf-8' });
            const url = URL.createObjectURL(blob);
            const link = document.createElement('a');
            link.href = url;
            link.download = filename;
            document.body.appendChild(link);
            link.click();
            link.remove();
            URL.revokeObjectURL(url);
        }

        function downloadDailyReport() {
            if (!latestDailyReport || latestDailyReport.rows.length === 0) {
                showToast('Generate a daily report with data first', 'warning');
                return;
            }
            downloadCsvFile(`daily-report-${latestDailyReport.date}.csv`, ['Department', 'Presents Count'], latestDailyReport.rows);
        }

        function downloadMonthlyReport() {
            if (!latestMonthlyReport || latestMonthlyReport.rows.length === 0) {
                showToast('Generate a monthly report with data first', 'warning');
                return;
            }
            downloadCsvFile(`monthly-report-${latestMonthlyReport.month}.csv`, ['Roll Number', 'Student Name', 'Present Days'], latestMonthlyReport.rows);
        }

        async function generateDailyReport() {
            const date = document.getElementById('report-daily-date').value;
            if (!date) {
                showToast('Please select a date first', 'warning');
                return;
            }
            try {
                const stats = await apiRequest(`/api/reports/daily?date=${date}`);
                const div = document.getElementById('daily-report-output');
                const downloadBtn = document.getElementById('download-daily-report');
                div.innerHTML = '';

                const entries = Object.entries(stats);
                if (entries.length === 0) {
                    latestDailyReport = null;
                    downloadBtn.style.display = 'none';
                    div.innerHTML = `<p class="text-muted">No logs recorded for ${date}.</p>`;
                    return;
                }

                latestDailyReport = { date, rows: entries.map(([dept, count]) => [dept, count]) };
                downloadBtn.style.display = '';

                let html = `<table class="w-full"><thead><tr><th>Department</th><th>Presents Count</th></tr></thead><tbody>`;
                entries.forEach(([dept, count]) => {
                    html += `<tr><td><strong>${escapeHtml(dept)}</strong></td><td><span class="badge badge-info">${escapeHtml(count)}</span></td></tr>`;
                });
                html += `</tbody></table>`;
                div.innerHTML = html;
            } catch (err) {
                showToast('Report generation failed', 'danger');
            }
        }

        async function generateMonthlyReport() {
            const month = document.getElementById('report-monthly-month').value; // YYYY-MM
            if (!month) {
                showToast('Please select a month first', 'warning');
                return;
            }
            try {
                const stats = await apiRequest(`/api/reports/monthly?month=${month}`);
                const div = document.getElementById('monthly-report-output');
                const downloadBtn = document.getElementById('download-monthly-report');
                div.innerHTML = '';

                const entries = Object.entries(stats);
                if (entries.length === 0) {
                    latestMonthlyReport = null;
                    downloadBtn.style.display = 'none';
                    div.innerHTML = `<p class="text-muted">No attendance logs found for ${month}.</p>`;
                    return;
                }

                latestMonthlyReport = { month, rows: entries.map(([roll, data]) => [roll, data.name || '', data.days || 0]) };
                downloadBtn.style.display = '';

                let html = `<div style="max-height: 250px; overflow-y: auto;"><table class="w-full"><thead><tr><th>Roll Number</th><th>Student Name</th><th>Presents Count</th></tr></thead><tbody>`;
                entries.forEach(([roll, data]) => {
                    html += `<tr><td><code>${escapeHtml(roll)}</code></td><td><strong>${escapeHtml(data.name)}</strong></td><td><span class="badge badge-success">${escapeHtml(data.days)} days</span></td></tr>`;
                });
                html += `</tbody></table></div>`;
                div.innerHTML = html;
            } catch (err) {
                showToast('Report generation failed', 'danger');
            }
        }

        async function triggerBackup() {
            try {
                const res = await apiRequest('/api/backup', 'POST');
                if (res.status === 'success') {
                    showToast('Database backed up to SD Card backups folder', 'success');
                } else {
                    showToast('Backup failed', 'danger');
                }
            } catch (err) {
                showToast('Request failed', 'danger');
            }
        }

        // Settings View
        async function loadSettings() {
            try {
                const config = await apiRequest('/api/settings');
                document.getElementById('set-device-name').value = config.deviceName;
                document.getElementById('set-admin-user').value = config.adminUser;
                document.getElementById('set-admin-pass').value = ''; // Don't show password
                
                document.getElementById('set-wifi-ssid').value = config.wifiSSID;
                document.getElementById('set-wifi-pass').value = ''; // Don't show password
            } catch (err) {
                showToast('Failed to load system configurations', 'danger');
            }
        }

        document.getElementById('settings-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            const configData = {
                deviceName: document.getElementById('set-device-name').value.trim(),
                adminUser: document.getElementById('set-admin-user').value.trim()
            };

            const pass = document.getElementById('set-admin-pass').value;
            if (pass) {
                configData.adminPass = pass;
            }

            try {
                await apiRequest('/api/settings', 'POST', configData);
                showToast('System parameters saved successfully', 'success');
                loadSettings();
            } catch (err) {
                showToast('Failed to save settings', 'danger');
            }
        });

        document.getElementById('wifi-form').addEventListener('submit', async (e) => {
            e.preventDefault();
            const wifiData = {
                wifiSSID: document.getElementById('set-wifi-ssid').value.trim(),
                wifiPass: document.getElementById('set-wifi-pass').value
            };

            try {
                await apiRequest('/api/settings', 'POST', wifiData);
                showToast('Wi-Fi settings saved. Restart device to apply.', 'success');
                loadSettings();
            } catch (err) {
                showToast('Failed to save Wi-Fi settings', 'danger');
            }
        });

        async function syncTime() {
            const browserEpoch = Math.floor(Date.now() / 1000);
            try {
                const res = await apiRequest('/api/time', 'POST', { epoch: browserEpoch });
                if (res.status === 'success') {
                    showToast('System time synchronized successfully', 'success');
                    fetchDashboardData();
                } else {
                    showToast('Sync failed', 'danger');
                }
            } catch (err) {
                showToast('Failed to synchronize time', 'danger');
            }
        }

        async function restartDevice() {
            if (!confirm('Are you sure you want to restart the ESP32? (Dashboard connection will be lost)')) return;
            try {
                showToast('Restarting device...', 'warning');
                fetch('/api/restart', {
                    method: 'POST',
                    headers: { 'Authorization': 'Bearer ' + authToken }
                });
                setTimeout(() => { location.reload(); }, 5000);
            } catch (err) {
                // Ignore since server restarts
            }
        }

        // Initialize App & Background Polling
        function initApp() {
            switchPage('dashboard');
            loadActiveSession();
            loadSessionsLists();

            // Start regular background polling for live scans & card capture
            if (window.pollInterval) clearInterval(window.pollInterval);
            window.pollInterval = setInterval(() => {
                if (authToken && (currentView === 'dashboard' || scanCaptureActive)) {
                    fetchDashboardData();
                    loadActiveSession();
                }
            }, 1000);

            // Trigger sync automatically if ESP time is not set
            setTimeout(async () => {
                try {
                    const stats = await apiRequest('/api/dashboard');
                    if (stats.epoch < 1577836800) { // Year 2020 threshold
                        console.log('ESP time is unset. Autosyncing with browser clock...');
                        syncTime();
                    }
                } catch(e){}
            }, 1000);
        }

        // Boot
        window.addEventListener('load', () => {
            checkAuth();
            document.addEventListener('keydown', (e) => {
                if (e.key === 'Escape') closeStudentModal();
            });
        });
    </script>
</body>
</html>
)rawliteral";

#endif // WEB_PAGES_H
