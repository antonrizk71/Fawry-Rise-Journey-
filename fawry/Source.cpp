#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

// product main class

class Product {
protected:
    string name;
    double price;
    int quantity;

public:
    Product(string name, double price, int quantity) {
        this->name = name;
        this->price = price;
        this->quantity = quantity;
    }

    string getName() {
        return name;
    }

    double getPrice() {
        return price;
    }

    int getQuantity() {
        return quantity;
    }

    void reduceQuantity(int q) {
        if (q > quantity)
            throw runtime_error("Not enough quantity in stock.");
        quantity -= q;
    }

    virtual bool isExpired() const { return false; }
    virtual bool requiresShipping() const { return false; }
    virtual double getWeight() const { return 0.0; }
};

//  ExpirableProduct class

class ExpirableProduct : public Product {
    bool expired;
public:
    ExpirableProduct(string name, double price, int quantity, bool expired)
        : Product(name, price, quantity), expired(expired) {}


    bool isExpired() const override { return expired; }
};


class ShippableProduct : public Product {
    double weight;
public:
    ShippableProduct(string name, double price, int quantity, double weight)
        : Product(name, price, quantity), weight(weight) {}

    bool requiresShipping() const override { return true; }
    double getWeight() const override { return weight; }
};


class CartItem {
public:
    Product* product;
    int quantity;

    CartItem(Product* product, int quantity)
        : product(product), quantity(quantity) {}
};


class Cart {
    vector<CartItem> items;
public:
    void add(Product* product, int quantity) {
        if (quantity > product->getQuantity()) {
            throw runtime_error("Requested quantity exceeds available stock.");
        }
        items.push_back(CartItem(product, quantity));
    }

    bool isEmpty() const {
        return items.empty();
    }

    const vector<CartItem>& getItems() const {
        return items;
    }
};


class Customer {
public:
    string name;
    double balance;

    Customer(string name, double balance) {
        this->name = name;
        this->balance = balance;
    }

    void deduct(double amount) {
        if (balance < amount)
            throw runtime_error("Insufficient balance.");
        balance -= amount;
    }

    double getBalance() const {
        return balance;
    }
};


class ShippingService {
public:
    static void ship(const vector<CartItem>& items) {
        double totalWeight = 0.0;
        cout << "** Shipment notice **" << endl;
        for (const auto& item : items) {
            if (item.product->requiresShipping()) {
                cout << item.quantity << "x " << item.product->getName() << endl;
                double weight = item.product->getWeight() * item.quantity;
                cout << weight << "g" << endl;
                totalWeight += weight;
            }
        }
        if (totalWeight > 0)
            cout << "Total package weight " << totalWeight / 1000 << "kg" << endl;
    }
};


void checkout(Customer& customer, Cart& cart) {
    if (cart.isEmpty()) throw runtime_error("Cart is empty.");

    double subtotal = 0.0;
    double shipping = 0.0;
    vector<CartItem> shippables;

    for (const auto& item : cart.getItems()) {
        Product* p = item.product;
        if (p->isExpired()) throw runtime_error(p->getName() + " is expired.");
        if (item.quantity > p->getQuantity())
            throw runtime_error(p->getName() + " is out of stock.");

        subtotal += p->getPrice() * item.quantity;
        if (p->requiresShipping()) {
            shipping += 10.0; 
            shippables.push_back(item);
        }
    }

    double total = subtotal + shipping;
    customer.deduct(total);

    for (auto& item : cart.getItems()) {
        item.product->reduceQuantity(item.quantity);
    }

    if (!shippables.empty())
        ShippingService::ship(shippables);

    cout << "** Checkout receipt **" << endl;
    for (const auto& item : cart.getItems()) {
        cout << item.quantity << "x " << item.product->getName()
            << "\t" << item.product->getPrice() * item.quantity << endl;
    }
    cout << "----------------------" << endl;
    cout << "Subtotal\t" << subtotal << endl;
    cout << "Shipping\t" << shipping << endl;
    cout << "Amount\t\t" << total << endl;
    cout << "Remaining balance\t" << customer.getBalance() << endl;
}






int main() {
    try {
        Customer customer("Anton", 800);

        ExpirableProduct cheese("Cheese", 100, 5, false);
        ExpirableProduct biscuits("Biscuits", 150, 3, false);
        ShippableProduct tv("TV", 300, 4, 10000);
        Product scratchCard("Scratch Card", 50, 10);

        Cart cart;
        cart.add(&cheese, 2);
        cart.add(&biscuits, 1);
        cart.add(&tv, 1);
        cart.add(&scratchCard, 1);

        checkout(customer, cart);
    }
    catch (const exception& e) {
        cerr << "Checkout failed: " << e.what() << endl;
    }

    return 0;
}

