# 🧠 SBDFS – Second Brain Database File System

![2](https://github.com/user-attachments/assets/1eca63c0-4b04-4705-81b5-c78948433d70)


SBDFS is an **experimental FUSE-based virtual filesystem** designed to mount your MongoDB notes as a navigable file system. It is part of a broader vision to create a **Second Brain Database** — a system to externalize, organize, and interact with your thoughts and knowledge as if they were files on your computer.

> Imagine opening your terminal and `cd`-ing into your brain.

---

## 🚧 Status

SBDFS is currently a **work-in-progress** and may or may not evolve into a complete product. It is in the **exploration and prototyping stage**, with many design decisions still being refined.

---

## 🧩 Core Concepts

- **Flat Filesystem:**  
  SBDFS currently supports a **single-level** file structure, where all notes are stored directly under the mount point.
  
- **Symlink-Based Hierarchy:**  
  Instead of implementing a full multi-level filesystem in FUSE, SBDFS uses **symlinks + bash scripts** to simulate a hierarchical structure (e.g., organizing notes by tags, topics, or dates).
  
- **MongoDB Integration:**  
  All files correspond to documents in a MongoDB collection, making it easy to interact with notes via a standard filesystem interface.

---

## 🛠️ Features (WIP)

- [x] In-memory flat filesystem
- [ ] MongoDB-backed note loading
- [ ] Write support to sync changes back to MongoDB
- [ ] CLI tool to generate symlinks from metadata
- [ ] Bash automation for virtual directory trees
- [ ] Robust metadata xattr support

---

## 📦 Requirements

- Python 3.6+
- `fusepy`
- FUSE (installable via your OS package manager)
- MongoDB (local or remote instance)

---

## 🚀 Getting Started

```bash
# Install dependencies
pip install fusepy pymongo

# Clone the repo
git clone https://github.com/your-username/sbdfs.git
cd sbdfs

# Run the filesystem (replace /mnt/sbdfs with your desired mount point)
python main.py /mnt/sbdfs
```
