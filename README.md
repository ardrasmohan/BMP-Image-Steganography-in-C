# BMP-Image-Steganography-in-C
A C-based BMP image steganography project that hides and extracts secret data using the Least Significant Bit (LSB) technique.
# BMP Image Steganography in C

## Project Overview

This project implements **Image Steganography using C**, where secret information is hidden inside a BMP image without making noticeable changes to the original image.

The project uses the **Least Significant Bit (LSB)** technique to encode secret data into the pixel data of a BMP image.

The hidden data can later be extracted from the stego image using the decoding process.

---

## Objective

The main objective of this project is to understand and implement:

* File handling in C
* Structures and pointers
* Command-line arguments
* BMP image processing
* Bitwise operations
* LSB-based steganography
* Encoding and decoding techniques
* Dynamic file processing
* Error handling

---

##  What is Steganography?

**Steganography** is the technique of hiding secret information inside another file or medium in such a way that the existence of the hidden information is not obvious.

In this project, the secret data is hidden inside a **BMP image**.

The original image is called the **source image**, and the image containing the hidden information is called the **stego image**.

### Example

```text
Original BMP Image
        +
   Secret File
        ↓
   Encoding
        ↓
Stego BMP Image
        ↓
    Decoding
        ↓
  Secret File
```

---

## Steganography Technique Used

This project uses the **Least Significant Bit (LSB)** technique.

Each byte of secret data contains **8 bits**.

For every secret-data bit, one bit of the image pixel data is modified.

The least significant bit of an image byte is changed according to the secret bit.

### Example

Suppose an image byte is:

```text
10110110
```

To store a `1`:

```text
10110111
```

Only the least significant bit is changed.

Similarly, to store a `0`:

```text
10110110
```

Since only the LSB is modified, the visual difference in the image is generally not noticeable.

---

# Project Files

## `types.h`

Contains common data types and enumerations used throughout the project.

It defines:

* `uint`
* `Status`
* `OperationType`

Example:

```c
typedef enum
{
    e_success,
    e_failure
} Status;
```

---

## `encode.h`

Contains:

* Encoding-related constants
* `EncodeInfo` structure
* Function prototypes required for encoding

The `EncodeInfo` structure stores information such as:

* Source image filename
* Secret filename
* Secret file extension
* Secret file size
* Stego image filename
* File pointers
* Image capacity
* Image data buffers

---

## `decode.h`

Contains:

* Decoding-related constants
* `DecodeInfo` structure
* Function prototypes required for decoding

The `DecodeInfo` structure stores information required to extract the hidden data from the stego image.

---

## `test_encode.c`

This is the main driver program.

It:

1. Reads command-line arguments
2. Determines whether encoding or decoding is requested
3. Initializes the required structure
4. Calls the encoding or decoding functions
5. Displays the result

---

## `encode.c`

Contains the complete encoding implementation.

Major operations include:

* Opening input and output files
* Checking BMP image capacity
* Copying the BMP header
* Encoding the magic string
* Encoding the secret file extension
* Encoding the secret file size
* Encoding the secret file data
* Copying remaining image data

---

## `decode.c`

Contains the complete decoding implementation.

Major operations include:

* Opening the stego image
* Decoding the magic string
* Decoding the secret file extension
* Decoding the secret file size
* Decoding the secret file data
* Creating the output secret file

---

# Encoding Process

The encoding process follows these steps:

```text
Source BMP Image
       |
       ↓
Open Source Image
       |
       ↓
Check Image Capacity
       |
       ↓
Open Secret File
       |
       ↓
Copy BMP Header
       |
       ↓
Encode Magic String
       |
       ↓
Encode Secret File Extension
       |
       ↓
Encode Secret File Size
       |
       ↓
Encode Secret File Data
       |
       ↓
Copy Remaining Image Data
       |
       ↓
Create Stego Image
```

---

# Decoding Process

The decoding process follows these steps:

```text
Stego BMP Image
       |
       ↓
Open Stego Image
       |
       ↓
Decode Magic String
       |
       ↓
Decode Secret File Extension
       |
       ↓
Decode Secret File Size
       |
       ↓
Decode Secret File Data
       |
       ↓
Create Output File
       |
       ↓
Recover Secret Data
```

---

# Data Stored Inside the Image

The encoded information is stored in the following order:

```text
Magic String
      ↓
Secret File Extension
      ↓
Secret File Size
      ↓
Secret File Data
```

The decoder follows the same order to recover the original information.

---

# 💻 Compilation

Open the terminal in the project directory and compile the project using:

```bash
gcc test_encode.c encode.c decode.c -o steganography
```

---

# Running the Program

## Encoding

Use:

```bash
./steganography -e beautiful.bmp secret.txt stego_img.bmp
```

Where:

* `-e` → Encoding operation
* `beautiful.bmp` → Source BMP image
* `secret.txt` → File containing secret information
* `stego_img.bmp` → Output stego image

---

## Decoding

Use:

```bash
./steganography -d stego_img.bmp
```

The program extracts the hidden information from the stego image.

---

# Example

### Input

```text
Source Image:
beautiful.bmp

Secret File:
secret.txt
```

### Encoding

```bash
./steganography -e beautiful.bmp secret.txt stego_img.bmp
```

### Output

```text
stego_img.bmp
```

The secret information is now hidden inside the stego image.

### Decoding

```bash
./steganography -d stego_img.bmp
```

The hidden secret file is recovered.

---

# Capacity Calculation

The image must have enough pixel data to store the complete secret information.

The required capacity is calculated using:

```text
(2 + strlen(extension) + 1 + 4 + secret_file_size) × 8
```

Where:

* `2` → Magic string size
* `strlen(extension)` → Secret file extension
* `1` → Null character
* `4` → Secret file size
* `secret_file_size` → Actual secret data

Each byte of secret information requires **8 image bytes** because one bit is stored in each image byte.

---

#  Important Concepts Used

## 1. Bitwise AND

```c
image_buffer[i] & 0xFE
```

`0xFE` is:

```text
11111110
```

This clears the least significant bit.

---

## 2. Bitwise OR

```c
((data >> (7 - i)) & 1)
```

The required bit from the secret data is extracted and inserted into the image byte.

---

## 3. LSB Encoding

The basic encoding operation is:

```c
image_buffer[i] =
    (image_buffer[i] & 0xFE) |
    ((data >> (7 - i)) & 1);
```

This modifies only the least significant bit of the image byte.

---

# Technologies Used

* **Language:** C
* **Image Format:** BMP
* **Compiler:** GCC
* **IDE:** Visual Studio Code
* **Technique:** LSB Steganography
* **Platform:** Linux / macOS / Windows with GCC




