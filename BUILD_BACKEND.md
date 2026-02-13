# Build and run the backend

## Windows

### Option A: MinGW (no Visual Studio) – recommended

**Requires:** [MSYS2](https://www.msys2.org/) with MinGW64.

1. Install MSYS2, then in an MSYS2 terminal run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-make
   ```
2. From PowerShell or cmd in the **project root**:
   ```powershell
   cd backend
   powershell -ExecutionPolicy Bypass -File build_mingw.ps1
   ```
3. Start the backend (from project root):
   ```powershell
   start_backend.bat
   ```
   Or: `start_backend_mingw.bat` (MinGW only).  
   `start_backend.bat` tries MinGW build first, then Visual Studio build.

4. Verify: `npm run test:integration`

### Option B: Visual Studio 2022

## 1. Install C++ toolchain (one-time)

**Option A – Visual Studio 2022**  
- Download: https://visualstudio.microsoft.com/downloads/  
- Run installer → select workload **"Desktop development with C++"**  
- Install. Restart your terminal (or PC) if needed.

**Option B – Build Tools only**  
- https://visualstudio.microsoft.com/visual-cpp-build-tools/  
- Install "Desktop development with C++".

---

## 2. Build the backend

Open a **new** terminal (so it sees Visual Studio). From the project root:

```powershell
cd "D:\project gggg\laala\lala-store\backend"
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

If the `backend\build` folder was used before with another generator (e.g. Ninja), delete it first:

```powershell
Remove-Item -Recurse -Force backend\build
```

Then run the two `cmake` commands again.

---

## 3. Start the backend (Windows)

From project root:

```powershell
start_backend.bat
```

You should see something like:

- `Connected to SQLite database: ...`
- `Starting LALA STORE server on http://localhost:8001`

---

## 4. Verify

In another terminal:

```powershell
cd "D:\project gggg\laala\lala-store"
npm run test:integration
```

All checks should pass. Then open http://localhost:3001 and use the app; product details will load from the backend.

---

## 5. Use the app now (without backend)

- Frontend is already configured to work with **fallback (demo) data** when the backend is offline.
- If the dev server is running, open **http://localhost:3001** and click any product; you should see full details and a “Showing demo product (backend offline)” notice.
- If port 3001 is free, start the frontend with:  
  `cd frontend ; npm run dev`

---

## macOS / Linux

1. Install deps (e.g. macOS: `brew install cmake sqlite3`; Ubuntu: `sudo apt install build-essential cmake libsqlite3-dev`).
2. From project root:
   ```bash
   cd backend
   chmod +x build.sh
   ./build.sh
   ```
3. Start the backend (from project root):
   ```bash
   chmod +x start_backend.sh
   ./start_backend.sh
   ```
4. Verify: `npm run test:integration`
