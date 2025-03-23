local a = 9
local b = 33
print(a + b)
print(a)
print(b)

-- Fonction qui additionne deux nombres
function addition(a, b)
    return a + b
end

-- Fonction qui vérifie si un nombre est pair
function estPair(n)
    return n % 2 == 0
end

-- Fonction qui affiche un message personnalisé
function afficherMessage(nom)
    print("Bonjour, " .. nom .. " ! Bienvenue en Lua.")
end

-- Exemples d'utilisation des fonctions
print("3 + 5 =", addition(3, 5))  -- Affiche : 3 + 5 = 8
print("4 est pair ?", estPair(4))  -- Affiche : true
afficherMessage("Alice")  -- Affiche : Bonjour, Alice ! Bienvenue en Lua.
