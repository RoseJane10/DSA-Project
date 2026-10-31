CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(40) UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role VARCHAR(15) NOT NULL CHECK (role IN ('admin','staff','user'))
);

-- Keep the role constraint compatible with both a fresh database and the earlier customer role.
ALTER TABLE users DROP CONSTRAINT IF EXISTS users_role_check;
UPDATE users SET role='user' WHERE role='customer';
ALTER TABLE users ADD CONSTRAINT users_role_check CHECK (role IN ('admin','staff','user'));

CREATE TABLE IF NOT EXISTS brands (
    id SERIAL PRIMARY KEY,
    name VARCHAR(80) UNIQUE NOT NULL,
    brand_type VARCHAR(30) NOT NULL
);

CREATE TABLE IF NOT EXISTS products (
    id SERIAL PRIMARY KEY,
    name VARCHAR(120) NOT NULL,
    category VARCHAR(60) NOT NULL,
    brand_id INTEGER NOT NULL REFERENCES brands(id),
    price NUMERIC(10,2) NOT NULL CHECK (price >= 0),
    stock INTEGER NOT NULL CHECK (stock >= 0)
);

CREATE TABLE IF NOT EXISTS orders (
    id SERIAL PRIMARY KEY,
    username VARCHAR(40) NOT NULL,
    total NUMERIC(10,2) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS order_items (
    id SERIAL PRIMARY KEY,
    order_id INTEGER REFERENCES orders(id) ON DELETE CASCADE,
    product_id INTEGER REFERENCES products(id),
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    unit_price NUMERIC(10,2) NOT NULL
);

-- Remove legacy generic sample products only when no saved order references them.
DELETE FROM products p
WHERE p.brand_id IN (SELECT id FROM brands WHERE name IN ('Nike','Samsung','Apple'))
  AND NOT EXISTS (SELECT 1 FROM order_items oi WHERE oi.product_id = p.id);
DELETE FROM brands b
WHERE b.name IN ('Nike','Samsung','Apple')
  AND NOT EXISTS (SELECT 1 FROM products p WHERE p.brand_id = b.id);

-- Initial administrator account: username admin, password admin123.
-- Change this password after first login; never commit real credentials.
CREATE EXTENSION IF NOT EXISTS pgcrypto;
INSERT INTO users(username, password_hash, role)
VALUES ('admin', crypt('admin123', gen_salt('bf')), 'admin')
ON CONFLICT (username) DO NOTHING;

INSERT INTO brands(name, brand_type) VALUES
('Maybelline','International Makeup'),
('L''Oreal Paris','International Makeup'),
('Fenty Beauty','Premium Makeup'),
('MAC Cosmetics','Premium Makeup'),
('COSRX','International Skincare'),
('CeraVe','International Skincare'),
('Happy Skin','Filipino Beauty'),
('BLK Cosmetics','Filipino Beauty'),
('Ever Bilena','Filipino Beauty'),
('GRWM Cosmetics','Filipino Beauty'),
('Dior Beauty','Luxury Beauty'),
('Laneige','International Skincare'),
('The Ordinary','International Skincare'),
('Beauty of Joseon','International Skincare'),
('Rare Beauty','Premium Makeup'),
('Charlotte Tilbury','Luxury Beauty'),
('Yves Saint Laurent Beauty','Luxury Beauty'),
('Issy','Filipino Beauty'),
('Sunnies Face','Filipino Beauty')
ON CONFLICT (name) DO NOTHING;

INSERT INTO products(name, category, brand_id, price, stock)
SELECT v.name, v.category, b.id, v.price, v.stock
FROM (VALUES
 ('Fit Me Matte Foundation','Makeup','Maybelline',399.00,20),
 ('Lifter Gloss','Makeup','Maybelline',449.00,15),
 ('True Match Foundation','Makeup','L''Oreal Paris',599.00,12),
 ('Pro Filt''r Foundation','Makeup','Fenty Beauty',2450.00,8),
 ('Studio Fix Powder','Makeup','MAC Cosmetics',2450.00,10),
 ('Low pH Good Morning Gel Cleanser','Skincare','COSRX',499.00,25),
 ('Moisturizing Cream','Skincare','CeraVe',850.00,12),
 ('Skin Tint','Makeup','Happy Skin',699.00,14),
 ('All-Day Lip Treatment','Makeup','BLK Cosmetics',299.00,18),
 ('Oil Control Powder','Makeup','Ever Bilena',199.00,22),
 ('Radiant Skin Foundation','Makeup','GRWM Cosmetics',499.00,16),
 ('Lip Glow','Makeup','Dior Beauty',2450.00,5),
 ('Water Bank Cream','Skincare','Laneige',1850.00,7),
 ('Niacinamide 10% Serum','Skincare','The Ordinary',650.00,10),
 ('Glow Deep Serum','Skincare','Beauty of Joseon',720.00,11),
 ('Soft Pinch Liquid Blush','Makeup','Rare Beauty',1650.00,6),
 ('Airbrush Flawless Foundation','Makeup','Charlotte Tilbury',3100.00,5),
 ('Rouge Pur Couture Lipstick','Makeup','Yves Saint Laurent Beauty',2900.00,4),
 ('Skin Tint','Makeup','Issy',599.00,13),
 ('Fluffmatte Lipstick','Makeup','Sunnies Face',395.00,17)
) AS v(name, category, brand, price, stock)
JOIN brands b ON b.name = v.brand
WHERE NOT EXISTS (SELECT 1 FROM products p WHERE p.name = v.name AND p.brand_id = b.id);
